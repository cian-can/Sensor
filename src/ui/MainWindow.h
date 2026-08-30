#pragma once
#include "common/SensorData.h"
#include "common/ThreadSafeQueue.h"
#include "business/AcquisitionManager.h"
#include "storage/SqliteStorage.h"
#include <QMainWindow>
#include <QTimer>
#include <memory>

class QLabel;
class QPushButton;
class QTextEdit;

// Qt Charts 类（Qt6 中位于全局命名空间）
class QChart;
class QChartView;
class QLineSeries;
class QValueAxis;

namespace sensor {

// ========== 主窗口（UI可视化层） ==========
// 对应文档2.2.1: 仅运行于Qt主线程，实时曲线、数字仪表盘、设备状态、告警展示
// 对应文档3.4.1: 帧率限制(30FPS)、滑动窗口、算力隔离
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // 初始化（注入采集管理器和存储）
    void setup(AcquisitionManager* acquisition, SqliteStorage* storage);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onStartClicked();
    void onStopClicked();
    void onDataReady(sensor::SensorFramePtr frame);
    void onAlarm(sensor::AlarmEvent alarm);
    void onStatusChanged(sensor::DeviceStatus status);
    void onStatsUpdated(uint64_t total, uint64_t dropped);
    void onUiRefresh();  // 30FPS UI刷新定时器

private:
    void setupUi();
    void setupChart();
    void updateValueLabels(const SensorFrame& frame);
    void appendChartData(const SensorFrame& frame);
    void setStatusIndicator(DeviceStatus status);
    QString formatTimestamp(uint64_t ms) const;

    // 核心组件
    AcquisitionManager* acquisition_ = nullptr;
    SqliteStorage*      storage_ = nullptr;

    // UI控件
    QPushButton* startBtn_ = nullptr;
    QPushButton* stopBtn_  = nullptr;
    QLabel*      statusIndicator_ = nullptr;
    QLabel*      statusText_ = nullptr;
    QLabel*      statsLabel_ = nullptr;

    QLabel* tempValueLabel_ = nullptr;
    QLabel* pressureValueLabel_ = nullptr;
    QLabel* humidityValueLabel_ = nullptr;
    QLabel* vibrationValueLabel_ = nullptr;

    QTextEdit* alarmLog_ = nullptr;

    // 图表
    QChart*     chart_ = nullptr;
    QChartView* chartView_ = nullptr;
    QLineSeries* tempSeries_ = nullptr;
    QLineSeries* pressureSeries_ = nullptr;
    QLineSeries* humiditySeries_ = nullptr;
    QLineSeries* vibrationSeries_ = nullptr;
    QValueAxis* axisX_ = nullptr;
    QValueAxis* axisY_ = nullptr;

    // UI刷新定时器（30FPS）
    QTimer* uiTimer_ = nullptr;

    // 待刷新数据缓冲（业务线程写入，UI定时器读取）
    ThreadSafeQueue<SensorFramePtr> uiBuffer_;

    // 曲线数据计数（滑动窗口）
    qint64  pointCounter_ = 0;
    int     maxPoints_ = 1000;
    uint64_t totalFrames_ = 0;
    uint64_t droppedFrames_ = 0;
};

} // namespace sensor
