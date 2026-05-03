#ifndef CPP_SIM_OOO_INTERNAL_HPP
#define CPP_SIM_OOO_INTERNAL_HPP

#include "common.hpp"
#include "memory.hpp"

#include <cstddef>

namespace riscv_sim {
namespace ooo_detail {

// 乱序模型的教学型固定容量。放在内部头文件中，供前端分配和后端调度共同使用。
// 这些值相当于硬件资源规模：容量越小，越容易触发结构相关 stall。
static const std::size_t ALU_RS_CAP = 8;
static const std::size_t LS_RS_CAP = 8;
static const std::size_t BR_RS_CAP = 4;
static const std::size_t ROB_CAP = 16;

// 判断一条指令是否需要写回架构寄存器；写 x0 被视为无效写回。
static inline bool writes_register(const DecodedInst &inst) {
    // Store/Branch/System 不写通用寄存器；写 x0 也被视为无效写回。
    // 乱序前端据此决定是否给目标寄存器加 tag。
    switch (inst.kind) {
    case OpcodeKind::Lui:
    case OpcodeKind::Auipc:
    case OpcodeKind::Jal:
    case OpcodeKind::Jalr:
    case OpcodeKind::Load:
    case OpcodeKind::OpImm:
    case OpcodeKind::Op:
        return inst.rd != 0;
    default:
        return false;
    }
}

static inline u32 load_value(Memory &mem, const DecodedInst &inst, u32 addr) {
    // funct3 决定 Load 宽度和有符号/无符号扩展方式。
    // signed load 需要 sign_extend；unsigned load 直接零扩展到 u32。
    switch (inst.funct3) {
    case 0x0: return static_cast<u32>(sign_extend(mem.read8(addr), 8));
    case 0x1: return static_cast<u32>(sign_extend(mem.read16(addr), 16));
    case 0x2: return mem.read32(addr);
    case 0x4: return mem.read8(addr);
    case 0x5: return mem.read16(addr);
    default: return 0;
    }
}

static inline bool store_value(Memory &mem, const DecodedInst &inst, u32 addr, u32 value) {
    // Store 宽度也由 funct3 决定；调用者保证只在 ROB 提交阶段真正写内存。
    // 返回 false 表示 funct3 不属于 SB/SH/SW，提交阶段会把它当成不支持指令。
    switch (inst.funct3) {
    case 0x0: mem.write8(addr, static_cast<u8>(value)); return true;
    case 0x1: mem.write16(addr, static_cast<u16>(value)); return true;
    case 0x2: mem.write32(addr, value); return true;
    default: return false;
    }
}

} // namespace ooo_detail
} // namespace riscv_sim

#endif
