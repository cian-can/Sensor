#pragma once
#include "common/SensorData.h"
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <atomic>
#include <cstdint>

namespace sensor {

// ========== 模拟传感器 TCP 服务端（测试/演示用） ==========
// 在本机监听端口，接受客户端连接后，按采样频率持续发送模拟传感器数据
// 模拟数据: 温度(正弦波+噪声)、压力、湿度、振动
class MockServer {
public:
    MockServer();
    ~MockServer();

    // 启动服务端（在独立线程运行）
    bool start(uint16_t port, int sampleRate = 50);

    // 停止
    void stop();

    bool isRunning() const { return running_; }

private:
    void run();
    void generateFrame(SensorFrame& frame);

    boost::asio::io_context ioContext_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::thread           thread_;
    std::atomic<bool>     running_{false};

    uint16_t port_ = 9000;
    int      sampleRate_ = 50;  // Hz
    uint64_t frameCount_ = 0;
    double   timeAccum_ = 0.0;  // 用于正弦波
};

} // namespace sensor
