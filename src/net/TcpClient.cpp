#include "net/TcpClient.h"
#include <spdlog/spdlog.h>
#include <QMetaType>
namespace sensor {

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
    , readBuffer_(4096) {
    // 注册自定义类型到Qt元对象系统（跨线程信号槽需要）
    qRegisterMetaType<sensor::SensorFramePtr>();
    qRegisterMetaType<sensor::DeviceStatus>();
}

TcpClient::~TcpClient() {
    stop();
}

void TcpClient::start(const std::string& host, uint16_t port) {
    if (running_.exchange(true)) {
        spdlog::warn("TcpClient已在运行中");
        return;
    }

    host_ = host;
    port_ = port;

    // 创建 work_guard，防止 io_context 无事可做时退出（文档3.1.1）
    workGuard_ = std::make_unique<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>(ioContext_.get_executor());

    // 启动独立IO线程
    ioThread_ = std::thread([this]() {
        spdlog::info("网络IO线程启动");
        try {
            ioContext_.run();
        } catch (const std::exception& e) {
            spdlog::error("网络IO线程异常: {}", e.what());
        }
        spdlog::info("网络IO线程退出");
    });

    // 发起连接（post到IO线程执行）
    boost::asio::post(ioContext_, [this]() { doConnect(); });
}

void TcpClient::stop() {
    if (!running_.exchange(false)) return;

    spdlog::info("停止TcpClient...");

    // post 停止操作到IO线程
    boost::asio::post(ioContext_, [this]() {
        closeSocket();
        if (reconnectTimer_) reconnectTimer_->cancel();
        if (heartbeatTimer_) heartbeatTimer_->cancel();
    });

    // 移除 work_guard，让 io_context 可以退出
    workGuard_.reset();

    // 停止 io_context
    ioContext_.stop();

    // 等待IO线程结束
    if (ioThread_.joinable()) {
        ioThread_.join();
    }

    // 重置状态（允许下次重新start）
    ioContext_.restart();
    setStatus(DeviceStatus::Disconnected);
    codec_.reset();
    {
        std::lock_guard<std::mutex> lock(writeMutex_);
        writeQueue_.clear();
    }
    reconnectCount_ = 0;

    spdlog::info("TcpClient已停止");
}

void TcpClient::send(const std::vector<uint8_t>& data) {
    if (!running_ || status_ != DeviceStatus::Connected) return;

    {
        std::lock_guard<std::mutex> lock(writeMutex_);
        writeQueue_.push_back(data);
    }

    // 通知IO线程有数据要写
    boost::asio::post(ioContext_, [this]() { doWrite(); });
}

// ==================== 连接 ====================
void TcpClient::doConnect() {
    if (!running_) return;

    setStatus(DeviceStatus::Connecting);
    spdlog::info("正在连接 {}:{} ...", host_, port_);

    socket_ = std::make_unique<boost::asio::ip::tcp::socket>(ioContext_);

    boost::asio::ip::tcp::resolver resolver(ioContext_);
    auto endpoints = resolver.resolve(host_, std::to_string(port_));

    boost::asio::async_connect(*socket_, endpoints,
        [this](const boost::system::error_code& ec,
               const boost::asio::ip::tcp::endpoint&) {
            onConnected(ec);
        });
}

void TcpClient::onConnected(const boost::system::error_code& ec) {
    if (ec) {
        spdlog::error("连接失败: {}", ec.message());
        closeSocket();
        setStatus(DeviceStatus::Error);
        emit networkError(QString::fromStdString(ec.message()));
        scheduleReconnect();
        return;
    }

    reconnectCount_ = 0;
    setStatus(DeviceStatus::Connected);
    spdlog::info("连接成功 {}:{}", host_, port_);

    codec_.reset();
    startHeartbeat();
    doRead();
}

// ==================== 异步读 ====================
void TcpClient::doRead() {
    if (!running_ || !socket_ || !socket_->is_open()) return;

    socket_->async_read_some(
        boost::asio::buffer(readBuffer_.data(), readBuffer_.size()),
        [this](const boost::system::error_code& ec, size_t bytes) {
            onRead(ec, bytes);
        });
}

void TcpClient::onRead(const boost::system::error_code& ec, size_t bytesTransferred) {
    if (ec) {
        if (ec == boost::asio::error::eof) {
            spdlog::warn("对端关闭连接");
        } else {
            spdlog::error("读取错误: {}", ec.message());
        }
        closeSocket();
        setStatus(DeviceStatus::Disconnected);
        scheduleReconnect();
        return;
    }

    // 数据送入协议编解码器
    codec_.append(readBuffer_.data(), bytesTransferred);

    // 尝试解析所有完整帧
    SensorFrame frame;
    while (codec_.tryDecode(frame)) {
        // 用 shared_ptr 包装，跨线程传递避免深拷贝（文档4.5）
        auto framePtr = std::make_shared<const SensorFrame>(std::move(frame));
        emit frameReceived(framePtr);
        frame = SensorFrame{};  // 重置
    }

    // 心跳处理
    if (codec_.hasHeartbeat()) {
        emit heartbeatReceived();
    }

    // 继续异步读
    doRead();
}

// ==================== 异步写 ====================
void TcpClient::doWrite() {
    if (!running_ || !socket_ || !socket_->is_open()) return;
    if (writing_.exchange(true)) return;  // 已有写操作在进行

    std::vector<uint8_t> data;
    {
        std::lock_guard<std::mutex> lock(writeMutex_);
        if (writeQueue_.empty()) {
            writing_ = false;
            return;
        }
        data = std::move(writeQueue_.front());
        writeQueue_.pop_front();
    }

    boost::asio::async_write(*socket_, boost::asio::buffer(data),
        [this](const boost::system::error_code& ec, size_t bytes) {
            onWrite(ec, bytes);
        });
}

void TcpClient::onWrite(const boost::system::error_code& ec, size_t /*bytes*/) {
    writing_ = false;
    if (ec) {
        spdlog::error("写入错误: {}", ec.message());
        closeSocket();
        setStatus(DeviceStatus::Disconnected);
        scheduleReconnect();
        return;
    }
    // 继续写队列中剩余数据
    doWrite();
}

// ==================== 重连 ====================
void TcpClient::scheduleReconnect() {
    if (!running_) return;

    // 检查重连次数限制
    if (maxReconnect_ > 0 && reconnectCount_ >= maxReconnect_) {
        spdlog::error("已达最大重连次数({})，停止重连", maxReconnect_);
        setStatus(DeviceStatus::Error);
        return;
    }

    ++reconnectCount_;
    setStatus(DeviceStatus::Reconnecting);
    spdlog::info("{}ms后发起第{}次重连...", reconnectInterval_, reconnectCount_);

    reconnectTimer_ = std::make_unique<boost::asio::steady_timer>(
        ioContext_, std::chrono::milliseconds(reconnectInterval_));
    reconnectTimer_->async_wait(
        [this](const boost::system::error_code& ec) { onReconnectTimer(ec); });
}

void TcpClient::onReconnectTimer(const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) return;
    if (!running_) return;
    doConnect();
}

// ==================== 心跳 ====================
void TcpClient::startHeartbeat() {
    if (heartbeatInterval_ <= 0) return;

    heartbeatTimer_ = std::make_unique<boost::asio::steady_timer>(
        ioContext_, std::chrono::milliseconds(heartbeatInterval_));
    heartbeatTimer_->async_wait(
        [this](const boost::system::error_code& ec) { onHeartbeatTimer(ec); });
}

void TcpClient::onHeartbeatTimer(const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) return;
    if (!running_ || status_ != DeviceStatus::Connected) return;

    // 发送心跳包
    auto hb = FrameCodec::encodeHeartbeat();
    send(hb);

    // 重启心跳定时器
    startHeartbeat();
}

// ==================== 工具 ====================
void TcpClient::setStatus(DeviceStatus s) {
    if (status_.load() != s) {
        status_ = s;
        emit statusChanged(s);
    }
}

void TcpClient::closeSocket() {
    if (socket_) {
        boost::system::error_code ec;
        socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket_->close(ec);
        socket_.reset();
    }
}

} // namespace sensor
