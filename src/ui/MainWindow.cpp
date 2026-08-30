#include "ui/MainWindow.h"
#include "common/Config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QSplitter>
#include <QFrame>
#include <QDateTime>
#include <QCloseEvent>
#include <QPainter>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <spdlog/spdlog.h>

namespace sensor {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , uiBuffer_(2048) {
    setupUi();
    setupChart();
}

MainWindow::~MainWindow() = default;

void MainWindow::setup(AcquisitionManager* acquisition, SqliteStorage* storage) {
    acquisition_ = acquisition;
    storage_ = storage;

    // 采集管理器信号 → 本窗口
    connect(acquisition_, &AcquisitionManager::dataReady,
            this, &MainWindow::onDataReady, Qt::QueuedConnection);
    connect(acquisition_, &AcquisitionManager::alarmOccurred,
            this, &MainWindow::onAlarm, Qt::QueuedConnection);
    connect(acquisition_, &AcquisitionManager::deviceStatusChanged,
            this, &MainWindow::onStatusChanged, Qt::QueuedConnection);
    connect(acquisition_, &AcquisitionManager::statsUpdated,
            this, &MainWindow::onStatsUpdated, Qt::QueuedConnection);

    // 数据同时写入存储
    connect(acquisition_, &AcquisitionManager::dataReady,
            storage_, &SqliteStorage::writeAsync, Qt::QueuedConnection);

    maxPoints_ = Config::instance().maxPoints;
}

void MainWindow::setupUi() {
    setWindowTitle("远程传感器数据采集及可视化系统");
    resize(1280, 800);

    // ===== 工具栏 =====
    QToolBar* toolBar = addToolBar("控制");
    toolBar->setMovable(false);

    startBtn_ = new QPushButton("开始采集", this);
    startBtn_->setStyleSheet("QPushButton{background:#27ae60;color:white;padding:6px 16px;border-radius:4px;font-weight:bold;}"
                              "QPushButton:hover{background:#2ecc71;}"
                              "QPushButton:disabled{background:#bdc3c7;}");
    stopBtn_ = new QPushButton("停止采集", this);
    stopBtn_->setStyleSheet("QPushButton{background:#e74c3c;color:white;padding:6px 16px;border-radius:4px;font-weight:bold;}"
                             "QPushButton:hover{background:#ec7063;}"
                             "QPushButton:disabled{background:#bdc3c7;}");
    stopBtn_->setEnabled(false);

    connect(startBtn_, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(stopBtn_, &QPushButton::clicked, this, &MainWindow::onStopClicked);

    toolBar->addWidget(startBtn_);
    toolBar->addWidget(stopBtn_);
    toolBar->addSeparator();

    // 状态指示灯
    statusIndicator_ = new QLabel(this);
    statusIndicator_->setFixedSize(16, 16);
    statusIndicator_->setStyleSheet("background:#95a5a6;border-radius:8px;");
    statusText_ = new QLabel("未连接", this);
    statusText_->setStyleSheet("font-size:13px;color:#7f8c8d;");
    toolBar->addWidget(statusIndicator_);
    toolBar->addWidget(statusText_);

    // ===== 中央区域: 上曲线 + 下数值/告警 =====
    QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);

    // 曲线上方
    chartView_ = new QChartView(this);
    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setMinimumHeight(400);
    mainSplitter->addWidget(chartView_);

    // 下方: 左数值面板 + 右告警日志
    QWidget* bottomWidget = new QWidget(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);

    // 数值面板
    QGroupBox* valueGroup = new QGroupBox("实时数值", bottomWidget);
    QGridLayout* valueGrid = new QGridLayout(valueGroup);
    valueGrid->setSpacing(12);

    auto createValueLabel = [](const QString& name, const QString& color) {
        QLabel* nameLabel = new QLabel(name);
        nameLabel->setStyleSheet(QString("font-size:14px;color:%1;font-weight:bold;").arg(color));
        return nameLabel;
    };

    valueGrid->addWidget(createValueLabel("温度", "#FF6B6B"), 0, 0);
    tempValueLabel_ = new QLabel("--.- ℃");
    tempValueLabel_->setStyleSheet("font-size:24px;font-weight:bold;color:#FF6B6B;");
    valueGrid->addWidget(tempValueLabel_, 0, 1);

    valueGrid->addWidget(createValueLabel("压力", "#4ECDC4"), 1, 0);
    pressureValueLabel_ = new QLabel("--.- kPa");
    pressureValueLabel_->setStyleSheet("font-size:24px;font-weight:bold;color:#4ECDC4;");
    valueGrid->addWidget(pressureValueLabel_, 1, 1);

    valueGrid->addWidget(createValueLabel("湿度", "#45B7D1"), 2, 0);
    humidityValueLabel_ = new QLabel("--.- %RH");
    humidityValueLabel_->setStyleSheet("font-size:24px;font-weight:bold;color:#45B7D1;");
    valueGrid->addWidget(humidityValueLabel_, 2, 1);

    valueGrid->addWidget(createValueLabel("振动", "#FFA07A"), 3, 0);
    vibrationValueLabel_ = new QLabel("--.- mm/s");
    vibrationValueLabel_->setStyleSheet("font-size:24px;font-weight:bold;color:#FFA07A;");
    valueGrid->addWidget(vibrationValueLabel_, 3, 1);

    bottomLayout->addWidget(valueGroup, 1);

    // 告警日志
    QGroupBox* alarmGroup = new QGroupBox("告警日志", bottomWidget);
    QVBoxLayout* alarmLayout = new QVBoxLayout(alarmGroup);
    alarmLog_ = new QTextEdit(alarmGroup);
    alarmLog_->setReadOnly(true);
    alarmLog_->setStyleSheet("QTextEdit{background:#2c3e50;color:#ecf0f1;font-family:Consolas,monospace;font-size:12px;}");
    alarmLayout->addWidget(alarmLog_);
    bottomLayout->addWidget(alarmGroup, 1);

    mainSplitter->addWidget(bottomWidget);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 2);

    setCentralWidget(mainSplitter);

    // ===== 状态栏 =====
    statsLabel_ = new QLabel("帧数: 0 | 丢弃: 0 | 已存储: 0", this);
    statusBar()->addWidget(statsLabel_);

    // ===== UI刷新定时器 (30FPS) =====
    uiTimer_ = new QTimer(this);
    connect(uiTimer_, &QTimer::timeout, this, &MainWindow::onUiRefresh);
    uiTimer_->start(1000 / 30);  // ~33ms = 30FPS
}

void MainWindow::setupChart() {
    chart_ = new QChart();
    chart_->setTitle("传感器实时数据曲线");
    chart_->setAnimationOptions(QChart::NoAnimation);  // 高频数据关闭动画提升性能

    // 创建4条曲线
    tempSeries_ = new QLineSeries();
    tempSeries_->setName("温度(℃)");
    tempSeries_->setColor(QColor("#FF6B6B"));
    tempSeries_->setUseOpenGL(true);  // 硬件加速（文档3.4.1）

    pressureSeries_ = new QLineSeries();
    pressureSeries_->setName("压力(kPa)");
    pressureSeries_->setColor(QColor("#4ECDC4"));
    pressureSeries_->setUseOpenGL(true);

    humiditySeries_ = new QLineSeries();
    humiditySeries_->setName("湿度(%RH)");
    humiditySeries_->setColor(QColor("#45B7D1"));
    humiditySeries_->setUseOpenGL(true);

    vibrationSeries_ = new QLineSeries();
    vibrationSeries_->setName("振动(mm/s)");
    vibrationSeries_->setColor(QColor("#FFA07A"));
    vibrationSeries_->setUseOpenGL(true);

    chart_->addSeries(tempSeries_);
    chart_->addSeries(pressureSeries_);
    chart_->addSeries(humiditySeries_);
    chart_->addSeries(vibrationSeries_);

    // X轴: 采样点序号
    axisX_ = new QValueAxis();
    axisX_->setTitleText("采样点");
    axisX_->setLabelFormat("%d");
    chart_->addAxis(axisX_, Qt::AlignBottom);

    // Y轴: 数值
    axisY_ = new QValueAxis();
    axisY_->setTitleText("数值");
    axisY_->setRange(-20, 120);
    chart_->addAxis(axisY_, Qt::AlignLeft);

    // 关联轴
    tempSeries_->attachAxis(axisX_);
    tempSeries_->attachAxis(axisY_);
    pressureSeries_->attachAxis(axisX_);
    pressureSeries_->attachAxis(axisY_);
    humiditySeries_->attachAxis(axisX_);
    humiditySeries_->attachAxis(axisY_);
    vibrationSeries_->attachAxis(axisX_);
    vibrationSeries_->attachAxis(axisY_);

    chart_->legend()->setVisible(true);
    chart_->legend()->setAlignment(Qt::AlignTop);

    chartView_->setChart(chart_);
}

void MainWindow::onStartClicked() {
    if (!acquisition_) return;
    acquisition_->start();
    startBtn_->setEnabled(false);
    stopBtn_->setEnabled(true);
    spdlog::info("用户点击开始采集");
}

void MainWindow::onStopClicked() {
    if (!acquisition_) return;
    acquisition_->stop();
    startBtn_->setEnabled(true);
    stopBtn_->setEnabled(false);
    spdlog::info("用户点击停止采集");
}

void MainWindow::onDataReady(SensorFramePtr frame) {
    // 业务线程调用，仅放入UI缓冲，不直接操作控件（文档3.3双重保障）
    uiBuffer_.push(frame);
}

void MainWindow::onAlarm(AlarmEvent alarm) {
    QString color = (alarm.level == AlarmLevel::Critical) ? "#e74c3c" : "#f39c12";
    QString html = QString("<span style='color:%1'>[%2] 设备%3 %4: 值=%.2f 阈值=%.2f</span>")
        .arg(color)
        .arg(formatTimestamp(alarm.timestamp))
        .arg(alarm.deviceId, 4, 16, QChar('0'))
        .arg(QString::fromStdString(alarm.message))
        .arg(alarm.value)
        .arg(alarm.threshold);
    alarmLog_->append(html);

    // 限制日志行数
    if (alarmLog_->document()->blockCount() > 500) {
        QTextCursor cursor(alarmLog_->document());
        cursor.movePosition(QTextCursor::Start);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
    }
}

void MainWindow::onStatusChanged(DeviceStatus status) {
    setStatusIndicator(status);
    statusText_->setText(deviceStatusName(status));
}

void MainWindow::onStatsUpdated(uint64_t total, uint64_t dropped) {
    totalFrames_ = total;
    droppedFrames_ = dropped;
}

void MainWindow::onUiRefresh() {
    // 30FPS 定时从缓冲队列拉取数据更新UI（文档3.4.1帧率限制）
    SensorFramePtr frame;
    int processed = 0;
    const int maxPerFrame = 50;  // 单帧最多处理50条，避免UI卡顿

    while (processed < maxPerFrame) {
        auto opt = uiBuffer_.tryPop();
        if (!opt) break;
        frame = *opt;
        updateValueLabels(*frame);
        appendChartData(*frame);
        ++processed;
    }

    // 更新状态栏
    uint64_t stored = storage_ ? storage_->totalWritten() : 0;
    statsLabel_->setText(QString("帧数: %1 | 丢弃: %2 | 已存储: %3 | 缓冲: %4")
        .arg(totalFrames_)
        .arg(droppedFrames_)
        .arg(stored)
        .arg(uiBuffer_.size()));
}

void MainWindow::updateValueLabels(const SensorFrame& frame) {
    if (frame.hasType(SensorType::Temperature))
        tempValueLabel_->setText(QString::number(frame.getValue(SensorType::Temperature), 'f', 1) + " ℃");
    if (frame.hasType(SensorType::Pressure))
        pressureValueLabel_->setText(QString::number(frame.getValue(SensorType::Pressure), 'f', 1) + " kPa");
    if (frame.hasType(SensorType::Humidity))
        humidityValueLabel_->setText(QString::number(frame.getValue(SensorType::Humidity), 'f', 1) + " %RH");
    if (frame.hasType(SensorType::Vibration))
        vibrationValueLabel_->setText(QString::number(frame.getValue(SensorType::Vibration), 'f', 2) + " mm/s");
}

void MainWindow::appendChartData(const SensorFrame& frame) {
    ++pointCounter_;

    if (frame.hasType(SensorType::Temperature))
        tempSeries_->append(pointCounter_, frame.getValue(SensorType::Temperature));
    if (frame.hasType(SensorType::Pressure))
        pressureSeries_->append(pointCounter_, frame.getValue(SensorType::Pressure));
    if (frame.hasType(SensorType::Humidity))
        humiditySeries_->append(pointCounter_, frame.getValue(SensorType::Humidity));
    if (frame.hasType(SensorType::Vibration))
        vibrationSeries_->append(pointCounter_, frame.getValue(SensorType::Vibration));

    // 滑动窗口：超过最大点数移除最旧数据（文档3.4.1）
    if (tempSeries_->count() > maxPoints_) {
        tempSeries_->remove(0);
        pressureSeries_->remove(0);
        humiditySeries_->remove(0);
        vibrationSeries_->remove(0);
    }

    // 动态调整X轴范围
    axisX_->setRange(std::max(0LL, pointCounter_ - maxPoints_), pointCounter_ + 10);
}

void MainWindow::setStatusIndicator(DeviceStatus status) {
    QString color;
    switch (status) {
        case DeviceStatus::Connected:    color = "#27ae60"; break;  // 绿
        case DeviceStatus::Connecting:
        case DeviceStatus::Reconnecting: color = "#f39c12"; break;  // 橙
        case DeviceStatus::Error:         color = "#e74c3c"; break;  // 红
        default:                          color = "#95a5a6"; break;  // 灰
    }
    statusIndicator_->setStyleSheet(
        QString("background:%1;border-radius:8px;").arg(color));
}

QString MainWindow::formatTimestamp(uint64_t ms) const {
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ms));
    return dt.toString("HH:mm:ss.zzz");
}

void MainWindow::closeEvent(QCloseEvent* event) {
    spdlog::info("窗口关闭，停止所有子系统...");
    if (acquisition_ && acquisition_->isRunning()) {
        acquisition_->stop();
    }
    if (storage_ && storage_->isRunning()) {
        storage_->stop();
    }
    event->accept();
}

} // namespace sensor
