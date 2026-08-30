#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <QMetaType>

namespace sensor {

// ========== 传感器测点类型 ==========
enum class SensorType : uint8_t {
    Temperature = 0x01,  // 温度 ℃
    Pressure    = 0x02,  // 压力 kPa
    Humidity    = 0x03,  // 湿度 %RH
    Vibration   = 0x04,  // 振动 mm/s
    Unknown     = 0xFF
};

inline const char* sensorTypeName(SensorType t) {
    switch (t) {
        case SensorType::Temperature: return "温度";
        case SensorType::Pressure:    return "压力";
        case SensorType::Humidity:    return "湿度";
        case SensorType::Vibration:   return "振动";
        default:                       return "未知";
    }
}

inline const char* sensorTypeUnit(SensorType t) {
    switch (t) {
        case SensorType::Temperature: return "℃";
        case SensorType::Pressure:    return "kPa";
        case SensorType::Humidity:    return "%RH";
        case SensorType::Vibration:   return "mm/s";
        default:                       return "";
    }
}

// ========== 单个测点数据 ==========
struct MeasurePoint {
    SensorType type;
    float      value;
};

// ========== 标准化传感器数据帧（业务层通用结构体） ==========
struct SensorFrame {
    uint16_t              deviceId;     // 设备ID
    uint64_t              timestamp;    // 毫秒级时间戳
    std::vector<MeasurePoint> points;   // 本帧包含的测点

    // 便捷获取某类型测点值
    float getValue(SensorType t, float def = 0.0f) const {
        for (const auto& p : points)
            if (p.type == t) return p.value;
        return def;
    }

    bool hasType(SensorType t) const {
        for (const auto& p : points)
            if (p.type == t) return true;
        return false;
    }
};

// 跨线程传递用共享指针，避免深拷贝（文档4.5方案）
using SensorFramePtr = std::shared_ptr<const SensorFrame>;
using SensorFrameMutablePtr = std::shared_ptr<SensorFrame>;

// ========== 设备状态 ==========
enum class DeviceStatus : uint8_t {
    Disconnected = 0,
    Connecting   = 1,
    Connected    = 2,
    Reconnecting = 3,
    Error        = 4
};

inline const char* deviceStatusName(DeviceStatus s) {
    switch (s) {
        case DeviceStatus::Disconnected: return "已断开";
        case DeviceStatus::Connecting:   return "连接中";
        case DeviceStatus::Connected:    return "已连接";
        case DeviceStatus::Reconnecting: return "重连中";
        case DeviceStatus::Error:        return "异常";
        default:                          return "未知";
    }
}

// ========== 告警级别 ==========
enum class AlarmLevel : uint8_t {
    Normal  = 0,
    Warning = 1,
    Critical = 2
};

struct AlarmEvent {
    uint64_t    timestamp;
    uint16_t    deviceId;
    SensorType  sensorType;
    AlarmLevel  level;
    std::string message;
    float       value;
    float       threshold;
};

} // namespace sensor

// Qt 元类型声明（跨线程信号槽传递需要）
Q_DECLARE_METATYPE(sensor::DeviceStatus)
Q_DECLARE_METATYPE(sensor::AlarmEvent)
Q_DECLARE_METATYPE(sensor::SensorFramePtr)
