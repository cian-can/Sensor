# 林育环境监测系统 (ForestBreedMonitor)

**实验室育种环境监控平台**

基于 **C++17 + Boost.Asio + Qt6 + SQLite3 + spdlog + OpenCV** 实现的实验室育种环境监测与可视化上位机软件。支持温湿度/压力/振动等多传感器数据实时采集与曲线绘制、数据持久化存储、以及基于 OpenCV 的定时拍照记录（每2小时一张，自动保留1个月）。

---

## 一、运行基础

### 1.1 系统要求

| 项目 | 要求 |
|---|---|
| 操作系统 | Linux (Ubuntu 20.04/22.04, Debian 11+) |
| 编译器 | GCC 9+ / Clang 10+ (支持 C++17) |
| 构建工具 | CMake 3.16+ |
| 显示 | X11 / Wayland (Qt6 GUI 需要) |
| 摄像头 | USB 摄像头（定时拍照功能需要，无摄像头时软件仍可运行） |

### 1.2 第三方依赖

| 库 | 版本 | 用途 | 安装包 (Ubuntu/Debian) |
|---|---|---|---|
| Boost | 1.71+ | Boost.Asio 异步网络、PropertyTree 配置解析 | `libboost-all-dev` |
| Qt6 | 6.2+ | GUI 框架、信号槽、事件循环 | `qt6-base-dev` |
| Qt Charts | 6.2+ | 实时曲线绘制 | `libqt6charts6-dev` |
| SQLite3 | 3.31+ | 本地数据持久化 | `libsqlite3-dev` |
| spdlog | 1.5+ | 分级日志 | `libspdlog-dev` |
| OpenCV | 4.x+ | 摄像头拍照、图像保存 | `libopencv-dev` |

### 1.3 一键安装依赖

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake \
    libboost-all-dev \
    qt6-base-dev libqt6charts6-dev \
    libsqlite3-dev \
    libspdlog-dev \
    libopencv-dev
```

---

## 二、编译与运行

### 2.1 一键构建

```bash
cd ForestBreedMonitor
chmod +x build.sh
./build.sh
```

### 2.2 手动构建

```bash
cd ForestBreedMonitor
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 2.3 运行

```bash
cd build
./ForestBreedMonitor                # 使用默认 config.ini
./ForestBreedMonitor /path/to/custom.ini  # 指定配置文件
```

> 默认配置 `mock_sensor = true`，程序内置模拟传感器，无需真实硬件即可演示数据采集功能。
> 相机功能默认 `enabled = true`，程序启动后自动每2小时拍摄一张照片。无摄像头时会在日志中提示，不影响其他功能。

---

## 三、项目架构

### 3.1 整体架构（五层 + 相机模块）

```
┌─────────────────────────────────────────────────────────┐
│                    UI 可视化层 (Qt)                       │
│  MainWindow / Qt Charts 实时曲线 / 数值面板 / 告警日志    │
│  相机状态面板 / 立即拍照按钮                               │
│  仅运行于 Qt 主线程，子线程禁止直接操作 UI 控件            │
└──────────────────────────┬──────────────────────────────┘
                           │ 信号槽 (Qt::QueuedConnection)
        ┌──────────────────┴──────────────────┐
        ▼                                     ▼
┌───────────────────────┐         ┌───────────────────────┐
│  业务逻辑管理层        │         │  相机拍照模块 (OpenCV) │
│  AcquisitionManager    │         │  CameraManager         │
│  DataProcessor 滤波告警│         │  每2小时定时拍照        │
│  采集调度/数据分发      │         │  照片保留1个月自动清理  │
└───────────┬───────────┘         └───────────────────────┘
            │
┌───────────▼───────────┐
│  通信传输层 (Boost.Asio)│
│  TcpClient 异步TCP客户端 │
│  独立IO线程 / 心跳 / 重连 │
└───────────┬───────────┘
            │
┌───────────▼───────────┐
│  设备协议适配层         │
│  FrameCodec 编解码      │
│  CRC16校验 / 粘包分包   │
└───────────┬───────────┘
            │
┌───────────▼───────────┐
│  持久化 & 工具支撑层    │
│  SqliteStorage 异步批量  │
│  Config / Logger / 队列  │
└───────────────────────┘
```

### 3.2 目录结构

```
ForestBreedMonitor/
├── CMakeLists.txt          # CMake 跨平台构建脚本
├── build.sh                # Linux 一键构建脚本
├── config.ini              # 运行配置文件
├── README.md               # 本文档
└── src/
    ├── main.cpp            # 程序入口
    ├── common/             # 基础支撑层
    │   ├── SensorData.h        # 标准化数据结构体、告警、设备状态
    │   ├── ThreadSafeQueue.h   # 线程安全有界环形队列
    │   ├── Config.h/.cpp       # Boost.PropertyTree INI配置解析
    │   └── Logger.h/.cpp       # spdlog 分级日志封装
    ├── protocol/           # 协议适配层
    │   ├── Protocol.h          # 协议常量定义
    │   └── FrameCodec.h/.cpp   # 帧编解码、CRC16、粘包分包
    ├── net/                # 网络通信层
    │   ├── TcpClient.h/.cpp    # Boost.Asio 异步TCP客户端
    │   └── MockServer.h/.cpp   # 模拟传感器TCP服务端(测试用)
    ├── business/           # 业务逻辑层
    │   ├── DataProcessor.h/.cpp    # 滑动平均滤波、异常剔除、阈值告警
    │   └── AcquisitionManager.h/.cpp # 采集调度中枢（整合网络+处理）
    ├── storage/            # 持久化层
    │   └── SqliteStorage.h/.cpp   # 独立线程异步批量写入SQLite
    ├── camera/             # 相机拍照模块
    │   └── CameraManager.h/.cpp   # OpenCV定时拍照、1个月自动清理
    └── ui/                 # UI可视化层
        └── MainWindow.h/.cpp      # Qt主窗口、Qt Charts实时曲线
```

---

## 四、相机定时拍照功能

### 4.1 功能说明

| 特性 | 说明 |
|---|---|
| 拍照间隔 | 默认每 **2小时** 自动拍摄一张（可配置） |
| 照片保留 | 默认保留 **30天（1个月）**，超期自动删除（可配置） |
| 存储结构 | `photos/YYYY-MM-DD/HH_MM_SS.jpg`，按日期分目录 |
| 图像格式 | JPEG，质量默认90（可配置 0-100） |
| 手动拍照 | 工具栏"立即拍照"按钮，随时手动拍摄 |
| 状态显示 | 底部状态栏显示相机连接状态、最近拍照时间、照片总数 |
| 异常恢复 | 拍照失败自动尝试重连摄像头，不崩溃 |

### 4.2 自动清理机制

每次拍照成功后，`CameraManager` 会遍历 `photos/` 目录下所有日期子目录，检查每张照片的最后修改时间：
- 超过 `retention_days`（默认30天）的照片自动删除
- 空的日期目录一并删除
- 清理后自动更新照片计数

### 4.3 线程模型

相机管理器运行在**独立线程**中（通过 `QTimer` 定时触发），拍照操作（`cv::VideoCapture::read` + `cv::imwrite`）在相机线程执行，不阻塞 UI 主线程。拍照完成后通过 `Qt::QueuedConnection` 信号通知 UI 更新状态。

### 4.4 无摄像头环境

如果运行环境没有摄像头（如服务器/虚拟机），程序不会崩溃：
- 启动时日志提示"摄像头打开失败，将在定时任务中重试"
- 状态栏显示"相机: 未连接"
- 传感器采集、数据存储等其他功能完全正常
- 点击"立即拍照"会尝试重新打开摄像头

---

## 五、代码之间的关系

### 5.1 模块依赖关系（单向依赖，无循环）

```
main.cpp
  ├── Config / Logger (common)
  ├── AcquisitionManager (business)          ← UI 唯一交互的业务对象
  │     ├── TcpClient (net)
  │     │     └── FrameCodec (protocol)     ← 粘包分包/CRC校验
  │     │           └── SensorData (common)
  │     └── DataProcessor (business)         ← 滤波/异常剔除/告警
  │           └── SensorData (common)
  ├── SqliteStorage (storage)                ← 独立线程异步批量写入
  │     ├── ThreadSafeQueue (common)
  │     └── SensorData (common)
  ├── CameraManager (camera)                 ← OpenCV定时拍照（独立线程）
  └── MainWindow (ui)
        ├── AcquisitionManager (business)
        ├── SqliteStorage (storage)
        ├── CameraManager (camera)
        └── SensorData (common)
```

### 5.2 核心数据流（传感器数据）

```
传感器设备 (TCP字节流)
    │  Boost.Asio async_read_some
    ▼
TcpClient::onRead()                         [网络IO线程]
    │  FrameCodec::append() + tryDecode()
    ▼
FrameCodec                                  [网络IO线程]
    │  粘包分包 → CRC16校验 → 解析为 SensorFrame
    │  emit frameReceived(shared_ptr<const SensorFrame>)
    ▼
AcquisitionManager::onNetworkFrame()        [Qt主线程]
    │  统计计数 → DataProcessor::processFrame()
    ▼
DataProcessor                               [Qt主线程]
    │  滑动平均滤波 → 物理范围异常剔除 → 阈值告警(边沿触发)
    │  emit frameProcessed(...) + emit alarmRaised(...)
    ▼
AcquisitionManager::onProcessedFrame()      转发
    │  emit dataReady(...)
    ▼
    ├───────────────────────────────────────┐
    ▼                                       ▼
MainWindow::onDataReady()            SqliteStorage::writeAsync()
入UI缓冲队列(不直接操作控件)          入存储队列(独立线程消费)
    │                                       │
    ▼  QTimer 30FPS 定时拉取                 ▼  达到batchSize或flushInterval
MainWindow::onUiRefresh()             SqliteStorage::flushBatch()
更新数值标签 + 追加曲线数据            SQLite事务批量INSERT
    │
    ▼
Qt Charts 实时曲线渲染 (滑动窗口限maxPoints)
```

### 5.3 相机数据流

```
QTimer 超时 (每2小时)  /  用户点击"立即拍照"
    │  QMetaObject::invokeMethod 转发到相机线程
    ▼
CameraManager::onTimerTimeout()            [相机线程]
    │  1. cv::VideoCapture::read() 读取帧
    │  2. 按日期创建目录: photos/YYYY-MM-DD/
    │  3. cv::imwrite() 保存为 HH_MM_SS.jpg
    │  4. cleanupOldPhotos() 删除超过30天的照片
    ▼
emit photoCaptured(filePath, timestamp)
    │  Qt::QueuedConnection
    ▼
MainWindow::onPhotoCaptured()              [Qt主线程]
更新状态栏"最近拍照"时间，临时提示文件路径
```

### 5.4 线程模型（5类线程严格隔离）

| 线程 | 运行内容 | 生命周期 | 关键机制 |
|---|---|---|---|
| **Qt 主线程** | UI渲染、信号槽分发、事件循环 | 程序全程 | `QApplication::exec()` |
| **网络 IO 线程** | Boost.Asio `io_context::run()`、异步读写回调、重连/心跳定时器 | 采集期间 | `executor_work_guard` 守护，独立 `std::thread` |
| **数据库 IO 线程** | SQLite 批量写入、队列消费 | 程序全程（启动后） | 独立 `std::thread`，定时/批量唤醒 |
| **相机线程** | OpenCV 拍照、照片保存、过期清理 | 相机启用期间 | `QTimer` 定时触发，独立线程 |
| **模拟服务端线程** | MockServer 接受连接、定时发送模拟数据 | mock模式采集期间 | 独立 `std::thread` |

**线程间通信**：全部使用 `Qt::QueuedConnection` 信号槽 + `std::shared_ptr<const T>` 共享指针（避免深拷贝），**子线程绝不直接操作 UI 控件**。

---

## 六、通信协议格式

### 6.1 帧结构

```
┌──────────┬──────────┬──────────────────────┬────────┐
│ 4字节帧长 │ 2字节设备ID│     N字节载荷         │ 2字节CRC│
└──────────┴──────────┴──────────────────────┴────────┘
```

- **帧长**：uint32 小端，包含整个帧的总字节数（含帧长字段自身）
- **设备ID**：uint16 小端，`0xFFFF` 保留为心跳包
- **载荷**：`[4字节时间戳ms][1字节测点数][N×(1字节类型+4字节float值)]`
- **CRC**：CRC16-CCITT（多项式 0x1021，初值 0xFFFF），覆盖帧长+设备ID+载荷

### 6.2 传感器类型编码

| 类型值 | 名称 | 单位 |
|---|---|---|
| 0x01 | Temperature 温度 | ℃ |
| 0x02 | Pressure 压力 | kPa |
| 0x03 | Humidity 湿度 | %RH |
| 0x04 | Vibration 振动 | mm/s |

---

## 七、配置文件说明 (config.ini)

| 节 | 键 | 默认值 | 说明 |
|---|---|---|---|
| network | host | 127.0.0.1 | 传感器设备地址 |
| network | port | 9000 | 端口 |
| network | reconnect_interval | 3000 | 重连间隔(ms) |
| network | max_reconnect | 0 | 最大重连次数(0=无限) |
| network | heartbeat_interval | 5000 | 心跳间隔(ms) |
| acquisition | sample_rate | 50 | 模拟采样率(Hz) |
| acquisition | mock_sensor | true | 模拟模式开关 |
| ui | refresh_fps | 30 | UI刷新帧率 |
| ui | max_points | 1000 | 曲线滑动窗口大小 |
| alarm | temp_high/low | 80/-10 | 温度告警阈值 |
| alarm | pressure_high | 500 | 压力告警阈值 |
| storage | db_path | sensor_data.db | SQLite文件路径 |
| storage | flush_interval | 1000 | 批量写入间隔(ms) |
| storage | batch_size | 500 | 批量写入最大条数 |
| log | level | info | 日志级别 |
| log | file | forest_breed.log | 日志文件 |
| **camera** | **enabled** | **true** | **是否启用定时拍照** |
| **camera** | **device_index** | **0** | **摄像头设备索引** |
| **camera** | **interval_hours** | **2** | **拍照间隔（小时）** |
| **camera** | **retention_days** | **30** | **照片保留天数（1个月）** |
| **camera** | **photo_dir** | **photos** | **照片保存目录** |
| **camera** | **quality** | **90** | **JPG质量（0-100）** |

---

## 八、扩展方向

1. **多设备并发**：扩展 `AcquisitionManager` 维护 `map<deviceId, TcpClient>` 多会话
2. **MQTT 上云**：在业务层新增 MQTT 发布者，将环境数据和照片上传云端
3. **FFT 频谱分析**：在 `DataProcessor` 中增加 FFT 计算，适配振动类传感器
4. **照片延时摄影**：将每日照片合成为延时摄影视频（OpenCV VideoWriter）
5. **数据回放**：利用 `SqliteStorage::queryHistory()` 实现历史数据曲线回放
6. **CSV 导出**：增加查询结果导出为 CSV 文件功能
7. **进程分离**：将采集/存储/相机拆为独立后台服务，UI 通过本地 IPC 连接

---

## 九、设计要点总结

| 设计决策 | 解决的问题 |
|---|---|
| 独立 Boost.Asio IO 线程 | 网络IO不阻塞UI，杜绝UI卡顿 |
| executor_work_guard | 防止io_context空转退出，网络服务常驻 |
| 流式 FrameCodec + CRC | TCP粘包分包、非法报文过滤 |
| shared_ptr<const T> 跨线程 | 高频数据零拷贝传递，降低CPU |
| 30FPS QTimer + UI缓冲队列 | 帧率限制，避免高频数据导致UI过载 |
| 曲线滑动窗口 | 防止无限数据累积导致内存暴涨 |
| 独立 SQLite 线程 + 批量事务 | 单条IO阻塞采集，异步批量兼顾实时与持久化 |
| 告警边沿触发 | 避免同一告警频繁刷屏 |
| 断线自动重连 + 心跳 | 网络抖动/设备断电后自动恢复 |
| 独立相机线程 + QTimer | 拍照不阻塞UI，定时可靠 |
| 照片按日期分目录 + 自动清理 | 1个月保留期，磁盘空间可控 |
| 拍照失败自动重连摄像头 | 摄像头异常断开后自动恢复，不崩溃 |
