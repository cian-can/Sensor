#include "common/Config.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <spdlog/spdlog.h>

namespace sensor {

AppConfig Config::config_;
bool      Config::loaded_ = false;

bool Config::loadFromFile(const std::string& path, AppConfig& cfg) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(path, pt);

        // network
        cfg.host = pt.get<std::string>("network.host", cfg.host);
        cfg.port = static_cast<uint16_t>(pt.get<int>("network.port", cfg.port));
        cfg.reconnectInterval = pt.get<int>("network.reconnect_interval", cfg.reconnectInterval);
        cfg.maxReconnect = pt.get<int>("network.max_reconnect", cfg.maxReconnect);
        cfg.heartbeatInterval = pt.get<int>("network.heartbeat_interval", cfg.heartbeatInterval);

        // acquisition
        cfg.sampleRate = pt.get<int>("acquisition.sample_rate", cfg.sampleRate);
        cfg.mockSensor = pt.get<bool>("acquisition.mock_sensor", cfg.mockSensor);

        // ui
        cfg.refreshFps = pt.get<int>("ui.refresh_fps", cfg.refreshFps);
        cfg.maxPoints = pt.get<int>("ui.max_points", cfg.maxPoints);
        cfg.colorTemp = pt.get<std::string>("ui.color_temp", cfg.colorTemp);
        cfg.colorPressure = pt.get<std::string>("ui.color_pressure", cfg.colorPressure);
        cfg.colorHumidity = pt.get<std::string>("ui.color_humidity", cfg.colorHumidity);
        cfg.colorVibration = pt.get<std::string>("ui.color_vibration", cfg.colorVibration);

        // alarm
        cfg.tempHigh = pt.get<float>("alarm.temp_high", cfg.tempHigh);
        cfg.tempLow  = pt.get<float>("alarm.temp_low", cfg.tempLow);
        cfg.pressureHigh = pt.get<float>("alarm.pressure_high", cfg.pressureHigh);
        cfg.humidityHigh = pt.get<float>("alarm.humidity_high", cfg.humidityHigh);

        // storage
        cfg.dbPath = pt.get<std::string>("storage.db_path", cfg.dbPath);
        cfg.flushInterval = pt.get<int>("storage.flush_interval", cfg.flushInterval);
        cfg.batchSize = pt.get<int>("storage.batch_size", cfg.batchSize);

        // log
        cfg.logLevel = pt.get<std::string>("log.level", cfg.logLevel);
        cfg.logFile = pt.get<std::string>("log.file", cfg.logFile);
        cfg.logMaxSize = pt.get<int>("log.max_size", cfg.logMaxSize);
        cfg.logMaxFiles = pt.get<int>("log.max_files", cfg.logMaxFiles);

        // camera
        cfg.cameraEnabled = pt.get<bool>("camera.enabled", cfg.cameraEnabled);
        cfg.cameraDeviceIndex = pt.get<int>("camera.device_index", cfg.cameraDeviceIndex);
        cfg.cameraIntervalHours = pt.get<int>("camera.interval_hours", cfg.cameraIntervalHours);
        cfg.cameraRetentionDays = pt.get<int>("camera.retention_days", cfg.cameraRetentionDays);
        cfg.cameraPhotoDir = pt.get<std::string>("camera.photo_dir", cfg.cameraPhotoDir);
        cfg.cameraQuality = pt.get<int>("camera.quality", cfg.cameraQuality);

        return true;
    } catch (const std::exception& e) {
        spdlog::warn("配置文件解析失败，使用默认配置: {}", e.what());
        return false;
    }
}

void Config::init(const std::string& path) {
    if (!loaded_) {
        loadFromFile(path, config_);
        loaded_ = true;
    }
}

const AppConfig& Config::instance() {
    return config_;
}

} // namespace sensor
