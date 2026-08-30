# 远程传感器数据采集及可视化系统

基于 **C++17 + Boost.Asio + Qt6 + SQLite3 + spdlog** 实现的远程传感器数据采集与实时可视化上位机软件。严格遵循五层分层架构设计，支持 TCP 异步通信、自定义二进制协议、数据滤波告警、实时曲线绘制、异步批量持久化。

---

## 一、运行基础

### 1.1 系统要求

| 项目 | 要求 |
|---|---|
| 操作系统 | Linux (Ubuntu 20.04/22.04, Debian 11+) |
| 编译器 | GCC 9+ / Clang 10+ (支持 C++17) |
| 构建工具 | CMake 3.16+ |
| 显示 | X11 / Wayland (Qt6 GUI 需要) |

### 1.2 第三方依赖

| 库 | 版本 | 用途 | 安装包 (Ubuntu/Debian) |
|---|---|---|---|
| Boost | 1.71+ | Boost.Asio 异步网络、PropertyTree 配置解析 | `libboost-all-dev` |
| Qt6 | 6.2+ | GUI 框架、信号槽、事件循环 | `qt6-base-dev` |
| Qt Charts | 6.2+ | 实时曲线绘制 | `libqt6charts6-dev` |
| SQLite3 | 3.31+ | 本地数据持久化 | `libsqlite3-dev` |
| spdlog | 1.5+ | 分级日志 | `libspdlog-dev` |

### 1.3 一键安装依赖

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake \
    libboost-all-dev \
    qt6-base-dev libqt6charts6-dev \
    libsqlite3-dev \
    libspdlog-dev
```

---

## 二、编译与运行

### 2.1 一键构建

```bash
cd sensor_viz
chmod +x build.sh
./build.sh
```

构建脚本会自动检查依赖、执行 CMake 配置、并行编译。

### 2.2 手动构建

```bash
cd sensor_viz
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 2.3 运行

```bash
cd build
./SensorViz                # 使用默认 config.ini
./SensorViz /path/to/custom.ini  # 指定配置文件
```

> 默认配置 `mock_sensor = true`，程序会内置启动一个模拟传感器 TCP 服务端，无需真实硬件即可演示全部功能。
> 连接真实设备时，将 `mock_sensor` 改为 `false`，并配置正确的 `host` 和 `port`。

---

## 三、项目架构

### 3.1 五层分层架构

```
┌─────────────────────────────────────────────────────────┐
│                    UI 可视化层 (Qt)                       │
│  MainWindow / Qt Charts 实时曲线 / 数值面板 / 告警日志    │
│  仅运行于 Qt 主线程，子线程禁止直接操作 UI 控件            │
└──────────────────────────┬──────────────────────────────┘
                           │ 信号槽 (Qt::QueuedConnection)
┌──────────────────────────▼──────────────────────────────┐
│                 业务逻辑管理层 (C++ 核心)                  │
│  AcquisitionManager 采集调度 / DataProcessor 滤波告警      │
│  数据预处理、阈值告警、跨线程数据分发、任务启停控制          │
└──────────────────────────┬──────────────────────────────┘
                           │ 信号槽 / 共享指针
┌──────────────────────────▼──────────────────────────────┐
│                 通信传输层 (Boost.Asio)                    │
│  TcpClient 异步TCP客户端 / 独立IO线程 / 心跳保活 / 断线重连 │
│  executor_work_guard 守护IO上下文，不占用Qt主线程           │
└──────────────────────────┬──────────────────────────────┘
                           │ 原始字节流
┌──────────────────────────▼──────────────────────────────┐
│                 设备协议适配层                              │
│  FrameCodec 编解码 / CRC16校验 / TCP粘包分包处理            │
│  [4字节帧长][2字节设备ID][N字节载荷][2字节CRC]             │
└──────────────────────────┬──────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────┐
│              持久化 & 工具支撑层                           │
│  SqliteStorage 异步批量写入 / Config 配置 / Logger 日志    │
│  ThreadSafeQueue 线程安全队列 / SensorData 数据结构体      │
└─────────────────────────────────────────────────────────┘
```

### 3.2 目录结构

```
sensor_viz/
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
    │   └── AcquisitionManager.h/.cpp # 采集调度中枢、模块整合
    ├── storage/            # 持久化层
    │   └── SqliteStorage.h/.cpp   # 独立线程异步批量SQLite写入
    └── ui/                 # UI可视化层
        └── MainWindow.h/.cpp      # Qt主窗口、Qt Charts实时曲线
```

---

## 四、代码之间的关系

### 4.1 模块依赖关系（单向依赖，无循环）

```
main.cpp
  ├── Config (common)
  ├── Logger (common)
  ├── AcquisitionManager (business)
  │     ├── TcpClient (net)
  │     │     └── FrameCodec (protocol)
  │     │           └── SensorData (common)
  │     └── DataProcessor (business)
  │           └── SensorData (common)
  ├── SqliteStorage (storage)
  │     ├── ThreadSafeQueue (common)
  │     └── SensorData (common)
  └── MainWindow (ui)
        ├── AcquisitionManager (business)
        ├── SqliteStorage (storage)
        └── SensorData (common)
```

**依赖方向**：UI → Business → Net → Protocol → Common，Storage 和 Common 为横切支撑层，被各层依赖。**不存在反向依赖或循环依赖**。

### 4.2 核心数据流（从网络到界面）

```
传感器设备 (TCP字节流)
    │
    ▼
TcpClient::onRead()          [网络IO线程]  Boost.Asio异步读取原始字节
    │  调用 FrameCodec::append() + tryDecode()
    ▼
FrameCodec                    [网络IO线程]  粘包分包、CRC校验、解析为SensorFrame
    │  emit frameReceived(shared_ptr<const SensorFrame>)
    ▼
AcquisitionManager::onNetworkFrame()  [Qt主线程/业务线程]  统计计数
    │  调用 DataProcessor::processFrame()
    ▼
DataProcessor                 [业务处理]  滑动平均滤波 → 异常值剔除 → 阈值告警
    │  emit frameProcessed(...)  +  emit alarmRaised(...)
    ▼
AcquisitionManager::onProcessedFrame()  转发
    │  emit dataReady(...)
    ▼
    ├──────────────────────────────────┐
    ▼                                  ▼
MainWindow::onDataReady()       SqliteStorage::writeAsync()
[UI缓冲队列，不直接操作控件]      [独立存储线程，入队缓存]
    │                                  │
    ▼  QTimer 30FPS 定时拉取            ▼  定时/批量 flush
MainWindow::onUiRefresh()         SqliteStorage::flushBatch()
更新数值标签 + 追加曲线数据          SQLite事务批量INSERT
    │
    ▼
Qt Charts 实时曲线渲染 (Qt主线程)
```

### 4.3 线程模型

| 线程 | 运行内容 | 生命周期 | 关键机制 |
|---|---|---|---|
| **Qt 主线程** | UI渲染、信号槽分发、事件循环 | 程序全程 | `QApplication::exec()` |
| **网络 IO 线程** | Boost.Asio `io_context::run()`、异步读写回调、重连/心跳定时器 | 采集期间 | `executor_work_guard` 守护，独立 `std::thread` |
| **数据库 IO 线程** | SQLite 批量写入、队列消费 | 程序全程（启动后） | 独立 `std::thread`，条件变量/定时唤醒 |
| **模拟服务端线程** | MockServer 接受连接、定时发送模拟数据 | mock模式采集期间 | 独立 `std::thread` |

**线程间通信**：全部使用 `Qt::QueuedConnection` 信号槽 + `std::shared_ptr<const T>` 共享指针（避免深拷贝），**子线程绝不直接操作 UI 控件**。

### 4.4 关键类职责与交互

#### AcquisitionManager（业务中枢）
- **整合** TcpClient（网络）和 DataProcessor（处理）
- 对外提供 `start()` / `stop()` 统一接口
- 接收网络帧 → 转发给处理器 → 接收处理结果 → 分发给 UI 和存储
- 是 UI 层唯一需要直接交互的业务对象

#### TcpClient（网络客户端）
- 继承 `QObject`，可 emit 信号
- 独立 `io_context` 线程，`executor_work_guard` 防止空转退出
- `async_read_some` 循环读，`async_write` 队列写
- 断线自动重连（可配置次数/间隔），定时心跳
- 收到数据后用 `FrameCodec` 流式解析，解析出完整帧后 emit

#### FrameCodec（协议编解码）
- **无状态流式解析器**，内部维护接收缓冲区
- `append()` 追加字节，`tryDecode()` 尝试提取完整帧
- 自动处理 TCP 粘包（多帧连在一起）和分包（一帧分多次到达）
- CRC16 校验失败或帧长非法时，逐字节丢弃重新同步
- `encode()` 将 `SensorFrame` 序列化为二进制帧（发送/心跳用）

#### DataProcessor（数据处理器）
- 对每种传感器类型维护独立的滑动窗口，做滑动平均滤波
- 物理范围异常值剔除（如湿度 >100% 直接丢弃）
- 阈值告警判断（温度过高/过低、压力过高、湿度过高）
- 告警**边沿触发**（状态变化时才 emit），避免频繁刷屏

#### SqliteStorage（持久化）
- 独立线程，`ThreadSafeQueue` 缓存待写入数据
- **异步批量写入**：达到 batchSize 或 flushInterval 时，用 SQLite 事务批量 INSERT
- WAL 模式 + `synchronous=NORMAL` 优化写入性能
- 程序退出时等待队列中数据全部落盘，不丢数据
- 提供 `queryHistory()` 同步查询接口（历史回放用）

#### MainWindow（主窗口）
- Qt Charts 4条实时曲线（温度/压力/湿度/振动），OpenGL 硬件加速
- **30FPS 帧率限制**：QTimer 定时从 UI 缓冲队列拉取数据，而非每帧数据都触发渲染
- **滑动窗口**：曲线超过 maxPoints 自动移除最旧数据，防止内存暴涨
- 左侧数值面板（大号字体实时显示），右侧告警日志（彩色分级）
- 顶部工具栏（开始/停止按钮 + 连接状态指示灯），底部状态栏（统计信息）
- 关闭窗口时自动停止采集和存储，优雅退出

---

## 五、通信协议格式

### 5.1 帧结构

```
┌──────────┬──────────┬──────────────────────┬────────┐
│ 4字节帧长 │ 2字节设备ID│     N字节载荷         │ 2字节CRC│
└──────────┴──────────┴──────────────────────┴────────┘
```

- **帧长**：uint32 小端，包含整个帧的总字节数（含帧长字段自身）
- **设备ID**：uint16 小端，`0xFFFF` 保留为心跳包
- **载荷**：
  ```
  ┌──────────────┬──────────┬─────────────────────────────┐
  │ 4字节时间戳ms  │ 1字节测点数│ N × (1字节类型 + 4字节float值)│
  └──────────────┴──────────┴─────────────────────────────┘
  ```
- **CRC**：CRC16-CCITT（多项式 0x1021，初值 0xFFFF），覆盖帧长+设备ID+载荷

### 5.2 传感器类型编码

| 类型值 | 名称 | 单位 |
|---|---|---|
| 0x01 | Temperature 温度 | ℃ |
| 0x02 | Pressure 压力 | kPa |
| 0x03 | Humidity 湿度 | %RH |
| 0x04 | Vibration 振动 | mm/s |

---

## 六、配置文件说明 (config.ini)

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
| log | file | sensor_viz.log | 日志文件 |

---

## 七、扩展方向

1. **多设备并发**：当前为单设备，可扩展 `AcquisitionManager` 维护 `map<deviceId, TcpClient>` 多会话
2. **MQTT 上云**：在业务层新增 MQTT 发布者，将处理后数据同步上传云端
3. **FFT 频谱分析**：在 `DataProcessor` 中增加 FFT 计算模块，适配振动类传感器
4. **数据回放**：利用 `SqliteStorage::queryHistory()` 实现历史数据曲线回放
5. **CSV 导出**：增加查询结果导出为 CSV 文件功能
6. **进程分离**：将采集/存储拆为独立后台服务，UI 通过本地 IPC 连接（文档5.2方案）

---

## 八、设计要点总结

| 设计决策 | 解决的问题 | 对应文档章节 |
|---|---|---|
| 独立 Boost.Asio IO 线程 | 网络IO不阻塞UI，杜绝UI卡顿 | 3.1.1 / 4.1 |
| executor_work_guard | 防止io_context空转退出，网络服务常驻 | 3.1.1 |
| 流式 FrameCodec + CRC | TCP粘包分包、非法报文过滤 | 3.2 / 4.3 |
| shared_ptr<const T> 跨线程 | 高频数据零拷贝传递，降低CPU | 4.5 |
| 30FPS QTimer + UI缓冲队列 | 帧率限制，避免高频数据导致UI过载 | 3.4.1 / 4.2 |
| 曲线滑动窗口 | 防止无限数据累积导致内存暴涨 | 3.4.1 |
| 独立 SQLite 线程 + 批量事务 | 单条IO阻塞采集，异步批量兼顾实时与持久化 | 3.5 / 4.6 |
| 告警边沿触发 | 避免同一告警频繁刷屏 | 3.3 |
| 断线自动重连 + 心跳 | 网络抖动/设备断电后自动恢复 | 3.1.2 |
