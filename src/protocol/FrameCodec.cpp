#include "protocol/FrameCodec.h"
#include <cstring>
#include <spdlog/spdlog.h>

namespace sensor {

// ========== CRC16-CCITT (多项式 0x1021, 初值 0xFFFF) ==========
uint16_t FrameCodec::crc16(const uint8_t* data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

// ========== 编码 ==========
std::vector<uint8_t> FrameCodec::encode(const SensorFrame& frame) {
    // 载荷: [4字节时间戳][1字节测点数][N*测点]
    uint32_t payloadSize = protocol::PAYLOAD_FIXED_SIZE +
                           static_cast<uint32_t>(frame.points.size()) * protocol::POINT_SIZE;
    uint32_t totalSize = protocol::FRAME_FIXED_SIZE + payloadSize;

    std::vector<uint8_t> buf(totalSize);
    size_t pos = 0;

    // 1. 帧长（4字节，小端）
    uint32_t frameLen = totalSize;
    std::memcpy(buf.data() + pos, &frameLen, 4);
    pos += 4;

    // 2. 设备ID（2字节，小端）
    uint16_t devId = frame.deviceId;
    std::memcpy(buf.data() + pos, &devId, 2);
    pos += 2;

    // 3. 载荷 - 时间戳（4字节，取低32位毫秒）
    uint32_t ts = static_cast<uint32_t>(frame.timestamp & 0xFFFFFFFF);
    std::memcpy(buf.data() + pos, &ts, 4);
    pos += 4;

    // 4. 载荷 - 测点数（1字节）
    uint8_t pointCount = static_cast<uint8_t>(frame.points.size());
    buf[pos++] = pointCount;

    // 5. 载荷 - 各测点
    for (const auto& p : frame.points) {
        buf[pos++] = static_cast<uint8_t>(p.type);
        float val = p.value;
        std::memcpy(buf.data() + pos, &val, 4);
        pos += 4;
    }

    // 6. CRC16（覆盖: 帧长+设备ID+载荷，不含CRC自身）
    uint16_t crc = crc16(buf.data(), pos);
    std::memcpy(buf.data() + pos, &crc, 2);

    return buf;
}

std::vector<uint8_t> FrameCodec::encodeHeartbeat() {
    SensorFrame hb;
    hb.deviceId = protocol::HEARTBEAT_DEVICE_ID;
    hb.timestamp = 0;
    // 空载荷
    return encode(hb);
}

// ========== 解码 ==========
FrameCodec::FrameCodec() = default;

void FrameCodec::append(const uint8_t* data, size_t len) {
    if (!data || len == 0) return;
    buffer_.insert(buffer_.end(), data, data + len);

    // 缓冲区过大保护（异常情况）
    if (buffer_.size() > protocol::MAX_FRAME_SIZE * 2) {
        spdlog::warn("协议缓冲区过大({}字节)，强制重置", buffer_.size());
        reset();
    }
}

bool FrameCodec::readFrameLength(uint32_t& outLen) const {
    if (buffer_.size() < protocol::FRAME_HEADER_SIZE) return false;
    std::memcpy(&outLen, buffer_.data(), 4);
    return true;
}

bool FrameCodec::tryDecode(SensorFrame& outFrame) {
    while (true) {
        // 1. 数据不足4字节，等更多数据
        if (buffer_.size() < protocol::FRAME_HEADER_SIZE)
            return false;

        // 2. 读取帧长
        uint32_t frameLen = 0;
        std::memcpy(&frameLen, buffer_.data(), 4);

        // 3. 帧长合法性校验
        if (frameLen < protocol::MIN_FRAME_SIZE || frameLen > protocol::MAX_FRAME_SIZE) {
            spdlog::warn("非法帧长: {}，丢弃1字节重新同步", frameLen);
            buffer_.erase(buffer_.begin());  // 丢弃1字节，尝试重新同步
            continue;
        }

        // 4. 数据不足一整帧，等更多数据（分包处理）
        if (buffer_.size() < frameLen)
            return false;

        // 5. 整帧数据已到齐，校验CRC
        const uint8_t* frameData = buffer_.data();
        size_t crcCoverLen = frameLen - protocol::CRC_SIZE;
        uint16_t calcCrc = crc16(frameData, crcCoverLen);
        uint16_t recvCrc = 0;
        std::memcpy(&recvCrc, frameData + crcCoverLen, 2);

        if (calcCrc != recvCrc) {
            spdlog::warn("CRC校验失败: 计算={:04X} 接收={:04X}，丢弃1字节重新同步",
                         calcCrc, recvCrc);
            buffer_.erase(buffer_.begin());
            continue;
        }

        // 6. CRC通过，解析帧内容
        size_t pos = protocol::FRAME_HEADER_SIZE;

        // 设备ID
        uint16_t devId = 0;
        std::memcpy(&devId, frameData + pos, 2);
        pos += 2;

        // 心跳包特殊处理
        if (devId == protocol::HEARTBEAT_DEVICE_ID) {
            heartbeatPending_ = true;
            buffer_.erase(buffer_.begin(), buffer_.begin() + frameLen);
            continue;  // 继续尝试解析下一帧
        }

        outFrame.deviceId = devId;

        // 时间戳
        uint32_t ts = 0;
        std::memcpy(&ts, frameData + pos, 4);
        pos += 4;
        outFrame.timestamp = static_cast<uint64_t>(ts);

        // 测点数
        uint8_t pointCount = frameData[pos++];
        outFrame.points.clear();
        outFrame.points.reserve(pointCount);

        // 各测点
        for (uint8_t i = 0; i < pointCount; ++i) {
            MeasurePoint mp;
            mp.type = static_cast<SensorType>(frameData[pos++]);
            std::memcpy(&mp.value, frameData + pos, 4);
            pos += 4;
            outFrame.points.push_back(mp);
        }

        // 7. 从缓冲区移除已解析的帧
        buffer_.erase(buffer_.begin(), buffer_.begin() + frameLen);
        return true;
    }
}

bool FrameCodec::hasHeartbeat() {
    bool hb = heartbeatPending_;
    heartbeatPending_ = false;
    return hb;
}

void FrameCodec::reset() {
    buffer_.clear();
    heartbeatPending_ = false;
}

} // namespace sensor
