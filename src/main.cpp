// ============================================================
// 远程传感器数据采集及可视化程序 - 程序入口
// 技术栈: C++17 + Boost.Asio + Qt6 + Qt Charts + SQLite3 + spdlog
// 架构: 五层分层 (UI → Business → Net → Protocol → Storage/Common)
// ============================================================

#include "common/Config.h"
#include "common/Logger.h"
#include "business/AcquisitionManager.h"
#include "storage/SqliteStorage.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QDir>
#include <spdlog/spdlog.h>
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    // ===== 1. Qt 应用初始化 =====
    QApplication app(argc, argv);
    QApplication::setApplicationName("SensorViz");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("SensorViz");

    // 确定配置文件路径（优先命令行参数，其次当前目录）
    QString configPath = QDir::currentPath() + "/config.ini";
    if (argc > 1) {
        configPath = QString::fromLocal8Bit(argv[1]);
    }

    // ===== 2. 加载配置 =====
    sensor::Config::init(configPath.toStdString());
    const auto& cfg = sensor::Config::instance();

    // ===== 3. 初始化日志 =====
    sensor::Logger::init(cfg.logLevel, cfg.logFile,
                         cfg.logMaxSize, cfg.logMaxFiles);

    spdlog::info("========================================");
    spdlog::info("远程传感器数据采集及可视化系统启动");
    spdlog::info("配置文件: {}", configPath.toStdString());
    spdlog::info("目标设备: {}:{}", cfg.host, cfg.port);
    spdlog::info("模拟模式: {}", cfg.mockSensor ? "开启" : "关闭");
    spdlog::info("采样率: {}Hz", cfg.sampleRate);
    spdlog::info("UI刷新: {}FPS", cfg.refreshFps);
    spdlog::info("========================================");

    // ===== 4. 创建业务组件 =====
    // 采集管理器（业务中枢：网络 + 数据处理）
    auto acquisition = std::make_unique<sensor::AcquisitionManager>();
    acquisition->init(cfg.host, cfg.port, cfg.sampleRate, cfg.mockSensor);

    // 数据存储（独立线程，异步批量写入SQLite）
    auto storage = std::make_unique<sensor::SqliteStorage>();
    if (!storage->init(cfg.dbPath, cfg.flushInterval, cfg.batchSize)) {
        spdlog::error("数据库初始化失败，存储功能将不可用");
    } else {
        storage->start();
    }

    // ===== 5. 创建主窗口 =====
    sensor::MainWindow mainWindow;
    mainWindow.setup(acquisition.get(), storage.get());
    mainWindow.show();

    spdlog::info("主窗口已显示，等待用户操作...");

    // ===== 6. 运行 Qt 事件循环 =====
    int ret = app.exec();

    // ===== 7. 退出清理 =====
    spdlog::info("程序退出，正在清理资源...");

    if (acquisition->isRunning()) {
        acquisition->stop();
    }
    if (storage->isRunning()) {
        storage->stop();
    }

    spdlog::info("程序已正常退出, 退出码={}", ret);
    return ret;
}
