#pragma once
#include "common/SensorData.h"
#include "protocol/FrameCodec.h"
#include <QObject>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>
#include <string>
#include <atomic>
#include <thread>
#include <deque>

namespace sensor {

// ========== Boost.Asio 异步 TCP 客户端 ==========
// 对应文档3.1: 独立IO线程，executor_work_guard守护，异步非阻塞
// 对应文档4.1: 网络线程仅负责收发，UI操作通过信号槽抛至Qt主线程
class TcpClient : public QObject, public std::enable_shared_from_this<TcpClient> {
    Q_OBJECT
public:
    explicit TcpClient(QObject* parent = nullptr);
    ~TcpClient() override;

    // 启动网络线程并连接
    void start(const std::string& host, uint16_t port);

    // 停止（断开连接，停止IO线程）
    void stop();

    // 发送数据（线程安全）
    void send(const std::vector<uint8_t>& data);

    // 当前连接状态
    DeviceStatus status() const { return status_; }

signals:
    // 收到完整传感器数据帧（跨线程信号，shared_ptr避免拷贝）
    void frameReceived(sensor::SensorFramePtr frame);

    // 连接状态变化
    void statusChanged(sensor::DeviceStatus status);

    // 网络错误
    void networkError(const QString& message);

    // 收到心跳
    void heartbeatReceived();

private:
    // ---------- 连接 ----------
    void doConnect();
    void onConnected(const boost::system::error_code& ec);

    // ---------- 异步读 ----------
    void doRead();
    void onRead(const boost::system::error_code& ec, size_t bytesTransferred);

    // ---------- 异步写 ----------
    void doWrite();
    void onWrite(const boost::system::error_code& ec, size_t bytesTransferred);

    // ---------- 重连 ----------
    void scheduleReconnect();
    void onReconnectTimer(const boost::system::error_code& ec);

    // ---------- 心跳 ----------
    void startHeartbeat();
    void onHeartbeatTimer(const boost::system::error_code& ec);

    // ---------- 内部 ----------
    void setStatus(DeviceStatus s);
    void closeSocket();

    // IO上下文与线程
    boost::asio::io_context       ioContext_;
    std::unique_ptr<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>> workGuard_;
    std::thread                   ioThread_;

    // Socket
    std::unique_ptr<boost::asio::ip::tcp::socket> socket_;

    // 连接参数
    std::string host_;
    uint16_t    port_ = 0;
    int         reconnectInterval_ = 3000;  // ms
    int         maxReconnect_ = 0;           // 0=无限
    int         reconnectCount_ = 0;
    int         heartbeatInterval_ = 5000;   // ms

    // 定时器
    std::unique_ptr<boost::asio::steady_timer> reconnectTimer_;
    std::unique_ptr<boost::asio::steady_timer> heartbeatTimer_;

    // 协议编解码器
    FrameCodec codec_;

    // 读缓冲区
    std::vector<uint8_t> readBuffer_;

    // 写队列（线程安全）
    std::deque<std::vector<uint8_t>> writeQueue_;
    std::mutex                        writeMutex_;
    std::atomic<bool>                 writing_{false};

    // 状态
    std::atomic<DeviceStatus> status_{DeviceStatus::Disconnected};
    std::atomic<bool>         running_{false};
};

} // namespace sensor
