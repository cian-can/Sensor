#include "camera/CameraManager.h"
#include <spdlog/spdlog.h>
#include <QThread>
#include <QMetaObject>
#include <filesystem>
#include <chrono>
#include <algorithm>

namespace fs = std::filesystem;

namespace sensor {

CameraManager::CameraManager(QObject* parent)
    : QObject(parent) {
    timer_ = new QTimer(this);
    timer_->setSingleShot(false);
    connect(timer_, &QTimer::timeout, this, &CameraManager::onTimerTimeout);
}

CameraManager::~CameraManager() {
    stop();
}

void CameraManager::init(const std::string& photoDir,
                          int intervalHours,
                          int retentionDays,
                          int cameraIndex,
                          int quality,
                          bool enabled) {
    photoDir_ = photoDir;
    intervalHours_ = intervalHours > 0 ? intervalHours : 2;
    retentionDays_ = retentionDays > 0 ? retentionDays : 30;
    cameraIndex_ = cameraIndex;
    quality_ = (quality >= 0 && quality <= 100) ? quality : 90;
    enabled_ = enabled;

    // 确保照片根目录存在
    ensureDir(photoDir_);

    // 统计已有照片数
    photoCount_ = countPhotosRecursive(photoDir_);

    spdlog::info("相机管理器初始化: 目录={}, 间隔={}小时, 保留={}天, 设备索引={}, 质量={}, 启用={}",
                 photoDir_, intervalHours_, retentionDays_, cameraIndex_, quality_, enabled_);
    spdlog::info("已有照片数量: {}", photoCount_.load());
}

void CameraManager::start() {
    if (!enabled_) {
        spdlog::info("相机功能未启用，跳过启动");
        return;
    }
    if (running_.exchange(true)) return;

    spdlog::info("启动相机管理器...");

    // 尝试打开摄像头
    if (openCamera()) {
        cameraAvailable_ = true;
        emit cameraStatusChanged(true);
        spdlog::info("摄像头打开成功, 分辨率: {}x{}",
                     (int)capture_.get(cv::CAP_PROP_FRAME_WIDTH),
                     (int)capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    } else {
        cameraAvailable_ = false;
        emit cameraStatusChanged(false);
        spdlog::warn("摄像头打开失败，将在定时任务中重试");
    }

    // 启动定时器（间隔毫秒）
    int intervalMs = intervalHours_ * 3600 * 1000;
    timer_->start(intervalMs);
    spdlog::info("相机定时器已启动, 间隔={}小时 ({}ms)", intervalHours_, intervalMs);

    // 启动后立即拍一张作为初始记录
    QMetaObject::invokeMethod(this, &CameraManager::onTimerTimeout, Qt::QueuedConnection);
}

void CameraManager::stop() {
    if (!running_.exchange(false)) return;

    spdlog::info("停止相机管理器...");
    timer_->stop();
    closeCamera();
    cameraAvailable_ = false;
    emit cameraStatusChanged(false);
    spdlog::info("相机管理器已停止");
}

void CameraManager::captureNow() {
    if (!running_) {
        spdlog::warn("相机管理器未运行，无法立即拍照");
        emit captureFailed("相机管理器未运行");
        return;
    }
    // 转发到相机线程执行
    QMetaObject::invokeMethod(this, &CameraManager::onTimerTimeout, Qt::QueuedConnection);
}

QString CameraManager::lastCaptureTime() const {
    if (!lastCaptureTime_.isValid()) return "尚未拍照";
    return lastCaptureTime_.toString("yyyy-MM-dd HH:mm:ss");
}

void CameraManager::onTimerTimeout() {
    QString filePath, error;
    if (doCapture(filePath, error)) {
        lastCaptureTime_ = QDateTime::currentDateTime();
        ++photoCount_;

        QString ts = lastCaptureTime_.toString("yyyy-MM-dd HH:mm:ss");
        spdlog::info("拍照成功: {}", filePath.toStdString());

        emit photoCaptured(filePath, ts);
        emit cameraStatsUpdated(photoCount_.load(), ts);

        // 清理过期照片
        cleanupOldPhotos();
    } else {
        spdlog::warn("拍照失败: {}", error.toStdString());
        emit captureFailed(error);

        // 拍照失败可能是摄像头断开，尝试重新打开
        if (cameraAvailable_) {
            closeCamera();
            cameraAvailable_ = false;
            emit cameraStatusChanged(false);
        }
        if (openCamera()) {
            cameraAvailable_ = true;
            emit cameraStatusChanged(true);
            spdlog::info("摄像头重新打开成功");
        }
    }
}

bool CameraManager::doCapture(QString& outFilePath, QString& outError) {
    std::lock_guard<std::mutex> lock(cameraMutex_);

    // 确保摄像头已打开
    if (!capture_.isOpened()) {
        if (!openCamera()) {
            outError = "无法打开摄像头 (设备索引=" + QString::number(cameraIndex_) + ")";
            return false;
        }
    }

    // 读取帧（重试几次，因为第一帧可能是黑的）
    cv::Mat frame;
    bool ok = false;
    for (int i = 0; i < 3; ++i) {
        if (capture_.read(frame) && !frame.empty()) {
            ok = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!ok || frame.empty()) {
        outError = "读取摄像头帧失败";
        return false;
    }

    // 构建保存路径: photoDir/YYYY-MM-DD/HH_MM_SS.jpg
    QDateTime now = QDateTime::currentDateTime();
    std::string dateDir = photoDir_ + "/" + now.toString("yyyy-MM-dd").toStdString();
    if (!ensureDir(dateDir)) {
        outError = "无法创建日期目录: " + QString::fromStdString(dateDir);
        return false;
    }

    std::string fileName = now.toString("HH_mm_ss").toStdString() + ".jpg";
    std::string fullPath = dateDir + "/" + fileName;

    // 保存为JPG
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, quality_};
    if (!cv::imwrite(fullPath, frame, params)) {
        outError = "保存照片失败: " + QString::fromStdString(fullPath);
        return false;
    }

    outFilePath = QString::fromStdString(fullPath);
    return true;
}

bool CameraManager::openCamera() {
    if (capture_.isOpened()) return true;

    // 先尝试指定索引
    if (capture_.open(cameraIndex_)) {
        return true;
    }

    // 尝试默认设备
    if (capture_.open(0)) {
        cameraIndex_ = 0;
        return true;
    }

    return false;
}

void CameraManager::closeCamera() {
    if (capture_.isOpened()) {
        capture_.release();
    }
}

void CameraManager::cleanupOldPhotos() {
    if (!fs::exists(photoDir_)) return;

    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24 * retentionDays_);
    int deleted = 0;

    try {
        // 遍历日期目录
        for (const auto& dateEntry : fs::directory_iterator(photoDir_)) {
            if (!dateEntry.is_directory()) continue;

            bool dirHasFiles = false;
            for (const auto& fileEntry : fs::directory_iterator(dateEntry.path())) {
                if (!fileEntry.is_regular_file()) continue;

                auto ftime = fileEntry.last_write_time();
                // convert file_time_type to system_clock time (C++17 兼容方式)
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

                if (sctp < cutoff) {
                    fs::remove(fileEntry.path());
                    ++deleted;
                } else {
                    dirHasFiles = true;
                }
            }

            // 如果日期目录为空，删除目录
            if (!dirHasFiles) {
                fs::remove(dateEntry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        spdlog::warn("清理过期照片时出错: {}", e.what());
    }

    if (deleted > 0) {
        photoCount_ = std::max(0, photoCount_.load() - deleted);
        spdlog::info("清理过期照片: 删除 {} 张, 剩余 {} 张", deleted, photoCount_.load());
    }
}

int CameraManager::countPhotosRecursive(const std::string& dir) {
    if (!fs::exists(dir)) return 0;
    int count = 0;
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png") {
                    ++count;
                }
            }
        }
    } catch (const fs::filesystem_error&) {
        // 忽略
    }
    return count;
}

bool CameraManager::ensureDir(const std::string& path) {
    try {
        if (!fs::exists(path)) {
            return fs::create_directories(path);
        }
        return true;
    } catch (const fs::filesystem_error&) {
        return false;
    }
}

} // namespace sensor
