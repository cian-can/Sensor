#pragma once
#include "common/SensorData.h"
#include "net/TcpClient.h"
#include "business/DataProcessor.h"
#include <QObject>
#include <memory>

namespace sensor {

// ========== 采集管理器（业务中枢） ==========
// 对应文档2.2.2: 采集任务调度、跨线程数据分发、多设备状态管理、任务启停控制
// 职责: 整合网络层(TcpClient) + 业务层(DataProcessor)，统一对外提供采集启停接口
class AcquisitionManager : public QObject {
    Q_OBJECT
public:
    explicit AcquisitionManager(QObject* parent = nullptr);
    ~AcquisitionManager() override;

    // 初始化（配置参数）
    void init(const std::string& host, uint16_t port,
              int sampleRate, bool mockSensor);

    // 开始采集
    void start();

    // 停止采集
    void stop();

    // 是否正在采集
    bool isRunning() const { return running_; }

    // 获取数据处理器（用于连接信号槽）
    DataProcessor* processor() { return processor_.get(); }

    // 获取网络客户端
    TcpClient* network() { return client_.get(); }

signals:
    // 处理后的数据（转发自DataProcessor，供UI/存储订阅）
    void dataReady(sensor::SensorFramePtr frame);

    // 告警事件（转发）
    void alarmOccurred(sensor::AlarmEvent alarm);

    // 设备状态变化
    void deviceStatusChanged(sensor::DeviceStatus status);

    // 采集统计
    void statsUpdated(uint64_t totalFrames, uint64_t droppedFrames);

private slots:
    void onNetworkFrame(sensor::SensorFramePtr frame);
    void onProcessedFrame(sensor::SensorFramePtr frame);
    void onAlarm(sensor::AlarmEvent alarm);
    void onStatusChanged(sensor::DeviceStatus status);

private:
    std::unique_ptr<TcpClient>     client_;
    std::unique_ptr<DataProcessor> processor_;

    std::string host_;
    uint16_t    port_ = 0;
    int         sampleRate_ = 50;
    bool        mockSensor_ = true;

    std::atomic<bool> running_{false};

    // 统计
    std::atomic<uint64_t> totalFrames_{0};
    std::atomic<uint64_t> droppedFrames_{0};
};

} // namespace sensor
