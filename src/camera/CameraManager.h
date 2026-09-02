#pragma once
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <opencv2/opencv.hpp>
#include <string>
#include <atomic>
#include <mutex>
#include <memory>

namespace sensor {

// ========== 相机管理器（OpenCV 定时拍照） ==========
// 功能:
//   1. 每隔 interval_hours 小时自动拍摄一张照片
//   2. 照片按日期分目录存储: photo_dir/YYYY-MM-DD/HH_MM_SS.jpg
//   3. 自动删除超过 retention_days 天的照片（默认30天）
//   4. 独立线程运行，拍照不阻塞UI
//   5. 支持手动立即拍照
class CameraManager : public QObject {
    Q_OBJECT
public:
    explicit CameraManager(QObject* parent = nullptr);
    ~CameraManager() override;

    // 初始化（配置参数，不打开摄像头）
    void init(const std::string& photoDir,
              int intervalHours = 2,
              int retentionDays = 30,
              int cameraIndex = 0,
              int quality = 90,
              bool enabled = true);

    // 启动定时拍照（在独立线程中运行）
    void start();

    // 停止
    void stop();

    // 立即拍摄一张（线程安全，可从UI线程调用）
    void captureNow();

    // 状态查询
    bool isEnabled() const { return enabled_; }
    bool isCameraAvailable() const { return cameraAvailable_; }
    QString lastCaptureTime() const;  // "yyyy-MM-dd HH:mm:ss"
    int photoCount() const { return photoCount_; }
    QString photoDirectory() const { return QString::fromStdString(photoDir_); }

signals:
    // 拍照成功，返回文件路径和时间
    void photoCaptured(const QString& filePath, const QString& timestamp);

    // 相机状态变化（可用/不可用）
    void cameraStatusChanged(bool available);

    // 拍照失败
    void captureFailed(const QString& reason);

    // 统计信息更新（拍照计数）
    void cameraStatsUpdated(int totalPhotos, const QString& lastCapture);

private slots:
    void onTimerTimeout();

private:
    // 执行拍照并保存（在相机线程中调用）
    bool doCapture(QString& outFilePath, QString& outError);

    // 打开摄像头
    bool openCamera();

    // 关闭摄像头
    void closeCamera();

    // 清理过期照片（超过 retentionDays_ 天）
    void cleanupOldPhotos();

    // 统计照片总数
    int countPhotosRecursive(const std::string& dir);

    // 确保目录存在
    static bool ensureDir(const std::string& path);

    // 成员
    cv::VideoCapture capture_;
    QTimer*           timer_ = nullptr;

    std::string photoDir_;
    int    intervalHours_ = 2;
    int    retentionDays_ = 30;
    int    cameraIndex_ = 0;
    int    quality_ = 90;
    bool   enabled_ = true;

    std::atomic<bool> cameraAvailable_{false};
    std::atomic<bool> running_{false};
    std::atomic<int>  photoCount_{0};
    QDateTime          lastCaptureTime_;

    std::mutex         cameraMutex_;  // 保护 capture_ 的并发访问
};

} // namespace sensor
