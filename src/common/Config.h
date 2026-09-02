#pragma once
#include <string>
#include <cstdint>

namespace sensor {

// ========== 全局配置（基于 Boost.PropertyTree 解析 INI） ==========
// 对应文档3.6：统一管理设备地址、端口、采样频率、告警阈值等
struct AppConfig {
    // 网络
    std::string host = "127.0.0.1";
    uint16_t    port = 9000;
    int         reconnectInterval = 3000;  // ms
    int         maxReconnect = 0;           // 0=无限
    int         heartbeatInterval = 5000;   // ms

    // 采集
    int  sampleRate = 50;       // Hz
    bool mockSensor = true;     // 模拟模式

    // UI
    int    refreshFps = 30;
    int    maxPoints = 1000;
    std::string colorTemp = "#FF6B6B";
    std::string colorPressure = "#4ECDC4";
    std::string colorHumidity = "#45B7D1";
    std::string colorVibration = "#FFA07A";

    // 告警
    float tempHigh = 80.0f;
    float tempLow  = -10.0f;
    float pressureHigh = 500.0f;
    float humidityHigh = 90.0f;

    // 存储
    std::string dbPath = "sensor_data.db";
    int  flushInterval = 1000;  // ms
    int  batchSize = 500;

    // 日志
    std::string logLevel = "info";
    std::string logFile = "forest_breed.log";
    int  logMaxSize = 10;   // MB
    int  logMaxFiles = 5;

    // 相机
    bool        cameraEnabled = true;
    int         cameraDeviceIndex = 0;
    int         cameraIntervalHours = 2;   // 拍照间隔（小时）
    int         cameraRetentionDays = 30;  // 照片保留天数
    std::string cameraPhotoDir = "photos";
    int         cameraQuality = 90;
};

class Config {
public:
    // 从 INI 文件加载配置
    static bool loadFromFile(const std::string& path, AppConfig& cfg);

    // 获取单例配置
    static const AppConfig& instance();

    // 初始化（程序启动时调用一次）
    static void init(const std::string& path);

private:
    static AppConfig config_;
    static bool      loaded_;
};

} // namespace sensor
