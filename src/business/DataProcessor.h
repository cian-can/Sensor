#pragma once
#include "common/SensorData.h"
#include <QObject>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace sensor {

// ========== 数据处理器 ==========
// 对应文档2.2.2: 数据预处理（滤波、异常值剔除）、阈值告警判断
// 对应文档3.4.1: 算力隔离，滤波等耗时操作在子线程完成
class DataProcessor : public QObject {
    Q_OBJECT
public:
    explicit DataProcessor(QObject* parent = nullptr);
    ~DataProcessor() override;

    // 设置告警阈值
    void setThresholds(float tempHigh, float tempLow,
                       float pressureHigh, float humidityHigh);

    // 设置滑动窗口大小（滤波窗口）
    void setFilterWindowSize(size_t size);

public slots:
    // 处理原始数据帧（由网络线程/采集管理器调用）
    void processFrame(sensor::SensorFramePtr rawFrame);

signals:
    // 处理后的数据帧（已滤波、已校验）
    void frameProcessed(sensor::SensorFramePtr processedFrame);

    // 告警事件
    void alarmRaised(sensor::AlarmEvent alarm);

private:
    // 滑动平均滤波
    float slidingAverage(SensorType type, float newValue);

    // 异常值检测（超出物理范围返回true）
    bool isAnomaly(SensorType type, float value) const;

    // 阈值告警检查
    void checkAlarm(uint16_t deviceId, uint64_t timestamp,
                    SensorType type, float value);

    // 每个传感器类型的滑动窗口
    std::unordered_map<int, std::deque<float>> filterWindows_;
    size_t filterWindowSize_ = 5;

    // 告警阈值
    float tempHigh_ = 80.0f;
    float tempLow_  = -10.0f;
    float pressureHigh_ = 500.0f;
    float humidityHigh_ = 90.0f;

    // 告警抑制（避免同一告警频繁触发）
    std::unordered_map<int, bool> alarmActive_;
};

} // namespace sensor
