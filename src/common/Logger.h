#pragma once
#include <string>

namespace sensor {

// ========== 日志封装（基于 spdlog） ==========
// 对应文档3.6：分级记录网络日志、采集日志、告警日志、异常日志
class Logger {
public:
    // 初始化日志系统（程序启动时调用一次）
    static void init(const std::string& level = "info",
                     const std::string& file = "sensor_viz.log",
                     int maxSizeMb = 10,
                     int maxFiles = 5);

    // 便捷宏（使用 spdlog 全局接口）
    // 直接用 spdlog::info / spdlog::warn / spdlog::error 即可
};

} // namespace sensor
