#include "storage/SqliteStorage.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <cstring>

namespace sensor {

SqliteStorage::SqliteStorage(QObject* parent)
    : QObject(parent)
    , writeQueue_(4096) {
}

SqliteStorage::~SqliteStorage() {
    stop();
}

bool SqliteStorage::init(const std::string& dbPath, int flushIntervalMs, int batchSize) {
    dbPath_ = dbPath;
    flushIntervalMs_ = flushIntervalMs;
    batchSize_ = batchSize;

    // WAL模式提升并发写入性能
    int rc = sqlite3_open(dbPath_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        spdlog::error("无法打开数据库: {}", sqlite3_errmsg(db_));
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }

    // 性能优化设置
    sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(db_, "PRAGMA cache_size=10000;", nullptr, nullptr, nullptr);

    if (!createTable()) {
        return false;
    }

    spdlog::info("数据库初始化成功: {}", dbPath_);
    return true;
}

bool SqliteStorage::createTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS sensor_data ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp INTEGER NOT NULL,"
        "  device_id INTEGER NOT NULL,"
        "  sensor_type INTEGER NOT NULL,"
        "  value REAL NOT NULL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_sensor_ts ON sensor_data(timestamp);"
        "CREATE INDEX IF NOT EXISTS idx_sensor_type ON sensor_data(sensor_type);"
        "CREATE INDEX IF NOT EXISTS idx_sensor_device ON sensor_data(device_id);";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        spdlog::error("建表失败: {}", errMsg ? errMsg : "unknown");
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

void SqliteStorage::start() {
    if (!db_) {
        spdlog::error("数据库未初始化，无法启动存储线程");
        return;
    }
    if (running_.exchange(true)) return;

    thread_ = std::thread(&SqliteStorage::run, this);
    spdlog::info("数据库存储线程已启动, 批量间隔={}ms, 批量大小={}",
                 flushIntervalMs_, batchSize_);
}

void SqliteStorage::stop() {
    if (!running_.exchange(false)) return;

    spdlog::info("停止数据库存储线程，等待队列落盘...");

    // 先把队列中剩余数据全部刷盘
    flushBatch();

    if (thread_.joinable()) {
        thread_.join();
    }

    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }

    spdlog::info("数据库存储线程已停止, 总写入记录={}", totalWritten_.load());
}

void SqliteStorage::writeAsync(SensorFramePtr frame) {
    if (!running_ || !frame) return;
    writeQueue_.push(frame);
}

void SqliteStorage::run() {
    while (running_) {
        // 等待 flushInterval 或队列达到 batchSize
        auto start = std::chrono::steady_clock::now();
        bool timedOut = false;

        while (running_ && !timedOut) {
            if (writeQueue_.size() >= static_cast<size_t>(batchSize_)) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count();
            if (elapsed >= flushIntervalMs_) {
                timedOut = true;
            }
        }

        if (writeQueue_.size() > 0) {
            flushBatch();
        }
    }
}

void SqliteStorage::flushBatch() {
    if (!db_ || writeQueue_.empty()) return;

    // 批量插入（事务提升性能）
    char* errMsg = nullptr;
    sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char* insertSql =
        "INSERT INTO sensor_data (timestamp, device_id, sensor_type, value) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, insertSql, -1, &stmt, nullptr) != SQLITE_OK) {
        spdlog::error("预编译INSERT失败: {}", sqlite3_errmsg(db_));
        sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    size_t count = 0;
    while (count < static_cast<size_t>(batchSize_)) {
        auto frame = writeQueue_.tryPop();
        if (!frame) break;

        for (const auto& p : (*frame)->points) {
            sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>((*frame)->timestamp));
            sqlite3_bind_int(stmt, 2, static_cast<int>((*frame)->deviceId));
            sqlite3_bind_int(stmt, 3, static_cast<int>(p.type));
            sqlite3_bind_double(stmt, 4, static_cast<double>(p.value));

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                spdlog::warn("插入失败: {}", sqlite3_errmsg(db_));
            }
            sqlite3_reset(stmt);
            ++count;
            ++totalWritten_;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &errMsg);
    if (errMsg) {
        spdlog::error("事务提交失败: {}", errMsg);
        sqlite3_free(errMsg);
    }

    if (count > 0) {
        spdlog::debug("批量写入 {} 条记录, 累计 {}", count, totalWritten_.load());
        emit storageStats(totalWritten_.load(), writeQueue_.size());
    }
}

std::vector<SqliteStorage::HistoryRecord> SqliteStorage::queryHistory(
    uint64_t startTime, uint64_t endTime, SensorType type, int limit) {
    std::vector<HistoryRecord> results;
    if (!db_) return results;

    std::string sql =
        "SELECT timestamp, device_id, sensor_type, value FROM sensor_data "
        "WHERE timestamp >= ? AND timestamp <= ?";
    if (type != SensorType::Unknown) {
        sql += " AND sensor_type = ?";
    }
    sql += " ORDER BY timestamp ASC LIMIT ?";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        spdlog::error("查询预编译失败: {}", sqlite3_errmsg(db_));
        return results;
    }

    int idx = 1;
    sqlite3_bind_int64(stmt, idx++, static_cast<sqlite3_int64>(startTime));
    sqlite3_bind_int64(stmt, idx++, static_cast<sqlite3_int64>(endTime));
    if (type != SensorType::Unknown) {
        sqlite3_bind_int(stmt, idx++, static_cast<int>(type));
    }
    sqlite3_bind_int(stmt, idx++, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        HistoryRecord rec;
        rec.timestamp = static_cast<uint64_t>(sqlite3_column_int64(stmt, 0));
        rec.deviceId = static_cast<uint16_t>(sqlite3_column_int(stmt, 1));
        rec.type = static_cast<SensorType>(sqlite3_column_int(stmt, 2));
        rec.value = static_cast<float>(sqlite3_column_double(stmt, 3));
        results.push_back(rec);
    }

    sqlite3_finalize(stmt);
    return results;
}

} // namespace sensor
