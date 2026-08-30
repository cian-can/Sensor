#pragma once
#include <cstdint>
#include <cstddef>

namespace sensor {
namespace protocol {

// ========== 协议帧格式（文档3.2.1） ==========
// [4字节帧长][2字节设备ID][N字节载荷][2字节CRC16]
//
// 载荷内部结构:
// [4字节时间戳ms][1字节测点数][N*(1字节类型+4字节float值)]

constexpr uint32_t FRAME_HEADER_SIZE = 4;  // 帧长字段
constexpr uint32_t DEVICE_ID_SIZE    = 2;  // 设备ID字段
constexpr uint32_t CRC_SIZE          = 2;  // CRC校验字段
constexpr uint32_t FRAME_FIXED_SIZE  = FRAME_HEADER_SIZE + DEVICE_ID_SIZE + CRC_SIZE;

// 载荷固定部分: 4字节时间戳 + 1字节测点数
constexpr uint32_t PAYLOAD_FIXED_SIZE = 5;

// 单个测点大小: 1字节类型 + 4字节float
constexpr uint32_t POINT_SIZE = 5;

// 最大帧长限制（防止恶意/异常数据）
constexpr uint32_t MAX_FRAME_SIZE = 65536;

// 帧长字段包含自身，所以最小有效帧 = 固定头 + 最小载荷(时间戳+0测点)
constexpr uint32_t MIN_FRAME_SIZE = FRAME_FIXED_SIZE + PAYLOAD_FIXED_SIZE;

// 心跳包标识（设备ID=0xFFFF，载荷为空的特殊帧）
constexpr uint16_t HEARTBEAT_DEVICE_ID = 0xFFFF;

} // namespace protocol
} // namespace sensor
