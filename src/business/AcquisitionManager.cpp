#include "business/AcquisitionManager.h"
#include "net/MockServer.h"
#include "common/Config.h"
#include <spdlog/spdlog.h>
#include <thread>
#include <chrono>

namespace sensor {

// 模拟服务端全局实例（mock模式下使用）
static std::unique_ptr<MockServer> g_mockServer = nullptr;

AcquisitionManager::AcquisitionManager(QObject* parent)
    : QObject(parent) {
    client_ = std::make_unique<TcpClient>(this);
    processor_ = std::make_unique<DataProcessor>(this);

    // 网络层 → 数据处理器（跨线程信号槽）
    connect(client_.get(), &TcpClient::frameReceived,
            this, &AcquisitionManager::onNetworkFrame, Qt::QueuedConnection);

    // 数据处理器 → 本管理器（转发）
    connect(processor_.get(), &DataProcessor::frameProcessed,
            this, &AcquisitionManager::onProcessedFrame, Qt::QueuedConnection);
    connect(processor_.get(), &DataProcessor::alarmRaised,
            this, &AcquisitionManager::onAlarm, Qt::QueuedConnection);

    // 状态变化
    connect(client_.get(), &TcpClient::statusChanged,
            this, &AcquisitionManager::onStatusChanged, Qt::QueuedConnection);
}

AcquisitionManager::~AcquisitionManager() {
    stop();
}

void AcquisitionManager::init(const std::string& host, uint16_t port,
                                int sampleRate, bool mockSensor) {
    host_ = host;
    port_ = port;
    sampleRate_ = sampleRate;
    mockSensor_ = mockSensor;

    // 设置告警阈值（从配置）
    const auto& cfg = Config::instance();
    processor_->setThresholds(cfg.tempHigh, cfg.tempLow,
                               cfg.pressureHigh, cfg.humidityHigh);
    processor_->setFilterWindowSize(5);
}

void AcquisitionManager::start() {
    if (running_.exchange(true)) {
        spdlog::warn("采集已在运行中");
        return;
    }

    spdlog::info("启动采集管理器...");
    totalFrames_ = 0;
    droppedFrames_ = 0;

    // Mock模式: 先启动模拟传感器服务端
    if (mockSensor_) {
        g_mockServer = std::make_unique<MockServer>();
        if (!g_mockServer->start(port_, sampleRate_)) {
            spdlog::error("模拟传感器服务端启动失败");
            running_ = false;
            return;
        }
        // 等待服务端就绪
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // 启动网络客户端
    client_->start(host_, port_);
    spdlog::info("采集管理器已启动, 目标={}:{}, mock={}", host_, port_, mockSensor_);
}

void AcquisitionManager::stop() {
    if (!running_.exchange(false)) return;

    spdlog::info("停止采集管理器...");
    client_->stop();

    if (g_mockServer) {
        g_mockServer->stop();
        g_mockServer.reset();
    }

    spdlog::info("采集管理器已停止, 总帧数={}, 丢弃={}",
                 totalFrames_.load(), droppedFrames_.load());
}

void AcquisitionManager::onNetworkFrame(SensorFramePtr frame) {
    ++totalFrames_;
    // 交给数据处理器做滤波/告警
    processor_->processFrame(frame);

    // 定期上报统计（每100帧）
    if (totalFrames_ % 100 == 0) {
        emit statsUpdated(totalFrames_.load(), droppedFrames_.load());
    }
}

void AcquisitionManager::onProcessedFrame(SensorFramePtr frame) {
    // 转发给UI和存储层
    emit dataReady(frame);
}

void AcquisitionManager::onAlarm(AlarmEvent alarm) {
    emit alarmOccurred(alarm);
}

void AcquisitionManager::onStatusChanged(DeviceStatus status) {
    emit deviceStatusChanged(status);
}

} // namespace sensor
