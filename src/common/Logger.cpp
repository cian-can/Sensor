#include "common/Logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <vector>

namespace sensor {

void Logger::init(const std::string& level,
                  const std::string& file,
                  int maxSizeMb,
                  int maxFiles) {
    try {
        // 控制台输出（带颜色）
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::trace);

        // 滚动文件输出
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            file,
            static_cast<size_t>(maxSizeMb) * 1024 * 1024,
            static_cast<size_t>(maxFiles));
        fileSink->set_level(spdlog::level::trace);

        // 多 sink 组合
        std::vector<spdlog::sink_ptr> sinks = {consoleSink, fileSink};
        auto logger = std::make_shared<spdlog::logger>("sensor_viz", sinks.begin(), sinks.end());

        // 日志级别
        spdlog::level::level_enum lvl = spdlog::level::info;
        if (level == "trace")    lvl = spdlog::level::trace;
        else if (level == "debug")   lvl = spdlog::level::debug;
        else if (level == "info")    lvl = spdlog::level::info;
        else if (level == "warn")    lvl = spdlog::level::warn;
        else if (level == "error")   lvl = spdlog::level::err;
        else if (level == "critical") lvl = spdlog::level::critical;

        logger->set_level(lvl);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
        logger->flush_on(spdlog::level::warn);

        spdlog::set_default_logger(logger);
        spdlog::info("日志系统初始化完成, 级别={}, 文件={}", level, file);
    } catch (const spdlog::spdlog_ex& ex) {
        // 日志初始化失败时降级到仅控制台
        auto console = spdlog::stdout_color_mt("fallback");
        spdlog::set_default_logger(console);
        spdlog::error("日志系统初始化失败(降级控制台): {}", ex.what());
    }
}

} // namespace sensor
