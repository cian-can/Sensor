#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>
#include <cstddef>

namespace sensor {

// ========== 线程安全环形队列（有界，满则丢弃最旧数据） ==========
// 对应文档4.2：队列满时自动丢弃老旧数据，防止内存暴涨
template <typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t maxSize = 1024)
        : maxSize_(maxSize), droppedCount_(0) {}

    // 推入元素，队列满时丢弃最旧元素
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.size() >= maxSize_) {
            queue_.pop_front();
            ++droppedCount_;
        }
        queue_.push_back(std::move(item));
        cv_.notify_one();
    }

    // 阻塞等待弹出
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });
        T item = std::move(queue_.front());
        queue_.pop_front();
        return item;
    }

    // 非阻塞尝试弹出
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return std::nullopt;
        T item = std::move(queue_.front());
        queue_.pop_front();
        return item;
    }

    // 带超时的阻塞弹出
    std::optional<T> popWithTimeout(int timeoutMs) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                          [this] { return !queue_.empty(); })) {
            return std::nullopt;
        }
        T item = std::move(queue_.front());
        queue_.pop_front();
        return item;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    size_t droppedCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return droppedCount_;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.clear();
    }

private:
    mutable std::mutex      mutex_;
    std::condition_variable cv_;
    std::deque<T>           queue_;
    size_t                  maxSize_;
    size_t                  droppedCount_;
};

} // namespace sensor
