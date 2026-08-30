#include "net/MockServer.h"
#include "protocol/FrameCodec.h"
#include <spdlog/spdlog.h>
#include <cmath>
#include <random>
#include <chrono>

namespace sensor {

MockServer::MockServer() = default;

MockServer::~MockServer() {
    stop();
}

bool MockServer::start(uint16_t port, int sampleRate) {
    if (running_.exchange(true)) return false;

    port_ = port;
    sampleRate_ = sampleRate;
    frameCount_ = 0;
    timeAccum_ = 0.0;

    try {
        acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
            ioContext_,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_));
    } catch (const std::exception& e) {
        spdlog::error("模拟服务端启动失败: {}", e.what());
        running_ = false;
        return false;
    }

    thread_ = std::thread(&MockServer::run, this);
    spdlog::info("模拟传感器服务端已启动，监听端口 {}，采样率 {}Hz", port_, sampleRate_);
    return true;
}

void MockServer::stop() {
    if (!running_.exchange(false)) return;

    ioContext_.stop();
    if (acceptor_) {
        boost::system::error_code ec;
        acceptor_->close(ec);
    }
    if (thread_.joinable()) {
        thread_.join();
    }
    ioContext_.restart();
    spdlog::info("模拟传感器服务端已停止");
}

void MockServer::run() {
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::normal_distribution<float> noise(0.0f, 0.5f);

    auto interval = std::chrono::microseconds(1000000 / sampleRate_);

    while (running_) {
        try {
            // 等待客户端连接
            boost::asio::ip::tcp::socket socket(ioContext_);
            spdlog::info("模拟服务端等待客户端连接...");
            acceptor_->accept(socket);
            spdlog::info("模拟服务端客户端已连接");

            // 连接成功后持续发送数据
            while (running_ && socket.is_open()) {
                SensorFrame frame;
                generateFrame(frame);

                auto bytes = FrameCodec::encode(frame);
                boost::system::error_code ec;
                boost::asio::write(socket, boost::asio::buffer(bytes), ec);

                if (ec) {
                    spdlog::warn("模拟服务端发送失败: {}", ec.message());
                    break;
                }

                ++frameCount_;
                std::this_thread::sleep_for(interval);
            }

            boost::system::error_code ec;
            socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            socket.close(ec);
            spdlog::info("模拟服务端客户端断开，共发送 {} 帧", frameCount_);

        } catch (const std::exception& e) {
            if (running_) {
                spdlog::error("模拟服务端异常: {}", e.what());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }
}

void MockServer::generateFrame(SensorFrame& frame) {
    frame.deviceId = 0x0001;
    frame.timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());

    timeAccum_ += 1.0 / sampleRate_;

    // 温度: 25℃ 基础 + 正弦波动 + 噪声
    float temp = 25.0f + 10.0f * static_cast<float>(std::sin(timeAccum_ * 0.5))
                 + static_cast<float>(std::rand() % 100) / 100.0f - 0.5f;

    // 压力: 101.3kPa 基础 + 小幅波动
    float pressure = 101.3f + 5.0f * static_cast<float>(std::sin(timeAccum_ * 0.3))
                     + static_cast<float>(std::rand() % 50) / 100.0f;

    // 湿度: 60% 基础 + 波动
    float humidity = 60.0f + 15.0f * static_cast<float>(std::sin(timeAccum_ * 0.2 + 1.0))
                     + static_cast<float>(std::rand() % 80) / 100.0f;

    // 振动: 随机高频
    float vibration = 2.0f * std::abs(static_cast<float>(std::sin(timeAccum_ * 3.0)))
                      + static_cast<float>(std::rand() % 30) / 100.0f;

    frame.points = {
        {SensorType::Temperature, temp},
        {SensorType::Pressure,    pressure},
        {SensorType::Humidity,    humidity},
        {SensorType::Vibration,   vibration}
    };
}

} // namespace sensor
