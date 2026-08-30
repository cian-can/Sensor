#pragma once
#include "common/SensorData.h"
#include "protocol/Protocol.h"
#include <vector>
#include <cstdint>
#include <cstddef>

namespace sensor {

// ========== 帧编解码器 ==========
// 职责:
//   1. 将标准化 SensorFrame 编码为二进制字节流（发送用）
//   2. 从 TCP 字节流中解析完整帧（粘包/分包处理，文档3.2/4.3）
//   3. CRC16 校验，非法帧丢弃
class FrameCodec {
public:
    FrameCodec();
    ~FrameCodec() = default;

    // ---------- 编码 ----------
    // 将 SensorFrame 编码为完整二进制帧（含帧长+设备ID+载荷+CRC）
    static std::vector<uint8_t> encode(const SensorFrame& frame);

    // 编码心跳包
    static std::vector<uint8_t> encodeHeartbeat();

    // ---------- 解码（流式，处理TCP粘包分包） ----------
    // 追加接收到的原始字节到内部缓冲区
    void append(const uint8_t* data, size_t len);

    // 尝试从缓冲区解析出一帧完整数据
    // 返回 true 表示解析成功，outFrame 输出解析结果
    // 返回 false 表示数据不足或帧非法（内部会自动丢弃非法数据）
    bool tryDecode(SensorFrame& outFrame);

    // 是否有心跳包待处理
    bool hasHeartbeat();

    // 重置缓冲区（异常时清空，文档4.3方案）
    void reset();

    // 当前缓冲区大小
    size_t bufferSize() const { return buffer_.size(); }

    // ---------- CRC16 ----------
    static uint16_t crc16(const uint8_t* data, size_t len);

private:
    std::vector<uint8_t> buffer_;
    bool                 heartbeatPending_ = false;

    // 尝试读取帧长（前4字节）
    bool readFrameLength(uint32_t& outLen) const;
};

} // namespace sensor
