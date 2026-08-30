#include "DataProcessor.h"
#include <spdlog/spdlog.h>
#include <numeric>
#include <QMetaType>
namespace sensor {

DataProcessor::DataProcessor(QObject* parent)
    : QObject(parent) {
    qRegisterMetaType<sensor::AlarmEvent>();
}

DataProcessor::~DataProcessor() = default;

void DataProcessor::setThresholds(float tempHigh, float tempLow,
                                    float pressureHigh, float humidityHigh) {
    tempHigh_ = tempHigh;
    tempLow_ = tempLow;
    pressureHigh_ = pressureHigh;
    humidityHigh_ = humidityHigh;
}

void DataProcessor::setFilterWindowSize(size_t size) {
    filterWindowSize_ = size > 0 ? size : 1;
}

void DataProcessor::processFrame(SensorFramePtr rawFrame) {
    if (!rawFrame) return;

    // 创建可修改的副本
    auto processed = std::make_shared<SensorFrame>();
    processed->deviceId = rawFrame->deviceId;
    processed->timestamp = rawFrame->timestamp;

    for (const auto& point : rawFrame->points) {
        // 1. 异常值剔除
        if (isAnomaly(point.type, point.value)) {
            spdlog::debug("异常值剔除: 类型={} 值={}",
                          static_cast<int>(point.type), point.value);
            continue;
        }

        // 2. 滑动平均滤波
        float filtered = slidingAverage(point.type, point.value);

        processed->points.push_back({point.type, filtered});

        // 3. 阈值告警检查
        checkAlarm(rawFrame->deviceId, rawFrame->timestamp, point.type, filtered);
    }

    if (!processed->points.empty()) {
        emit frameProcessed(processed);
    }
}

float DataProcessor::slidingAverage(SensorType type, float newValue) {
    int key = static_cast<int>(type);
    auto& window = filterWindows_[key];

    window.push_back(newValue);
    if (window.size() > filterWindowSize_) {
        window.pop_front();
    }

    float sum = std::accumulate(window.begin(), window.end(), 0.0f);
    return sum / static_cast<float>(window.size());
}

bool DataProcessor::isAnomaly(SensorType type, float value) const {
    // 物理范围合理性检查
    switch (type) {
        case SensorType::Temperature:
            return value < -100.0f || value > 1000.0f;
        case SensorType::Pressure:
            return value < 0.0f || value > 10000.0f;
        case SensorType::Humidity:
            return value < 0.0f || value > 100.0f;
        case SensorType::Vibration:
            return value < 0.0f || value > 1000.0f;
        default:
            return false;
    }
}

void DataProcessor::checkAlarm(uint16_t deviceId, uint64_t timestamp,
                                 SensorType type, float value) {
    int key = static_cast<int>(type);
    bool& active = alarmActive_[key];

    AlarmLevel level = AlarmLevel::Normal;
    float threshold = 0.0f;
    std::string msg;

    switch (type) {
        case SensorType::Temperature:
            if (value > tempHigh_) {
                level = AlarmLevel::Critical;
                threshold = tempHigh_;
                msg = "温度过高";
            } else if (value < tempLow_) {
                level = AlarmLevel::Warning;
                threshold = tempLow_;
                msg = "温度过低";
            }
            break;
        case SensorType::Pressure:
            if (value > pressureHigh_) {
                level = AlarmLevel::Critical;
                threshold = pressureHigh_;
                msg = "压力过高";
            }
            break;
        case SensorType::Humidity:
            if (value > humidityHigh_) {
                level = AlarmLevel::Warning;
                threshold = humidityHigh_;
                msg = "湿度过高";
            }
            break;
        default:
            break;
    }

    // 告警状态变化时才触发（边沿触发，避免刷屏）
    if (level != AlarmLevel::Normal && !active) {
        active = true;
        AlarmEvent evt{
            timestamp, deviceId, type, level, msg, value, threshold
        };
        emit alarmRaised(evt);
        spdlog::warn("告警: {} 值={:.2f} 阈值={:.2f}", msg, value, threshold);
    } else if (level == AlarmLevel::Normal && active) {
        active = false;
        spdlog::info("告警恢复: {}", sensorTypeName(type));
    }
}

} // namespace sensor
