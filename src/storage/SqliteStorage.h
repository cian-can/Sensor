#pragma once
#include "common/SensorData.h"
#include "common/ThreadSafeQueue.h"
#include <QObject>
#include <sqlite3.h>
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <vector>

namespace sensor {

// ========== SQLite 异步批量存储 ==========
// 对应文档3.5: 异步批量落盘，独立数据库工作线程，缓存高频数据定时批量写入
// 对应文档4.6: 分离采集线程与数据库IO线程，兼顾实时性与持久化
class SqliteStorage : public QObject {
    Q_OBJECT
public:
    explicit SqliteStorage(QObject* parent = nullptr);
    ~SqliteStorage() override;

    // 初始化（打开数据库，建表）
    bool init(const std::string& dbPath,
              int flushIntervalMs = 1000,
              int batchSize = 500);

    // 启动存储线程
    void start();

    // 停止（等待队列中数据全部落盘）
    void stop();

    // 异步写入一帧数据（线程安全，非阻塞）
    void writeAsync(SensorFramePtr frame);

    // 同步查询历史数据（用于历史回放）
    struct HistoryRecord {
        uint64_t timestamp;
        uint16_t deviceId;
        SensorType type;
        float value;
    };
    std::vector<HistoryRecord> queryHistory(uint64_t startTime, uint64_t endTime,
                                              SensorType type = SensorType::Unknown,
                                              int limit = 10000);

    // 已写入总记录数
    uint64_t totalWritten() const { return totalWritten_; }

    bool isRunning() const { return running_; }

signals:
    void storageStats(uint64_t totalWritten, size_t pendingCount);

private:
    void run();
    void flushBatch();
    bool createTable();

    sqlite3* db_ = nullptr;
    std::string dbPath_;

    int flushIntervalMs_ = 1000;
    int batchSize_ = 500;

    // 待写入队列
    ThreadSafeQueue<SensorFramePtr> writeQueue_;

    std::thread       thread_;
    std::atomic<bool> running_{false};
    std::atomic<uint64_t> totalWritten_{0};
};

} // namespace sensor
