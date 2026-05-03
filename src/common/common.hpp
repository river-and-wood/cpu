#ifndef CPP_SIM_COMMON_HPP
#define CPP_SIM_COMMON_HPP

#include <cstdint>
#include <string>

namespace riscv_sim {

// 固定宽度整数别名，避免在指令字段解析时反复书写 std::uintXX_t。
// 模拟器必须精确知道整数宽度：RISC-V RV32 的寄存器和指令都是 32 位，
// 如果直接使用 int/long，宽度会随平台变化，不利于做指令级模拟。
typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;
typedef std::int32_t s32;

// 模拟器内存映射：低地址为 RAM，IO_TIMER 作为简单的停止/计时 MMIO。
// 程序一般从地址 0 开始加载；写 IO_TIMER 会让 Memory::stopped() 变为 true。
static const u32 RAM_SIZE = 128u * 1024u;
static const u32 IO_TIMER = 0x00030004u;

// RV32I/M 指令在模拟器内部使用的粗粒度类别。
// 具体操作再由 funct3/funct7/寄存器字段区分。
enum class OpcodeKind {
    Invalid,
    Lui,
    Auipc,
    Jal,
    Jalr,
    Branch,
    Load,
    Store,
    OpImm,
    Op,
    System
};

// 解码后的 RISC-V 指令字段。
// raw 保留原始 32 位机器码，其他字段对应指令编码中的 rd/rs/funct/立即数。
// 顺序 CPU 和乱序 CPU 共用这个结构：前者直接执行，后者还会把它转成 MicroOp。
struct DecodedInst {
    u32 raw;        // 原始 32 位指令字
    OpcodeKind kind; // 指令大类
    u8 opcode;      // opcode[6:0]
    u8 rd;          // 目的寄存器编号
    u8 rs1;         // 源寄存器 1 编号
    u8 rs2;         // 源寄存器 2 编号
    u8 funct3;      // funct3 字段
    u8 funct7;      // funct7 字段
    s32 imm;        // 已按指令格式拼接并符号扩展的立即数
};

// 将低 bits 位的补码值符号扩展到 s32。
// 例如 12 位立即数 0xfff 表示 -1，扩展后应得到 0xffffffff。
inline s32 sign_extend(u32 value, unsigned bits) {
    const u32 mask = 1u << (bits - 1u);
    // (value ^ mask) - mask 是常见的无分支符号扩展写法：
    // 最高符号位为 0 时基本保持原值；为 1 时减去 2^bits 得到负数。
    return static_cast<s32>((value ^ mask) - mask);
}

// 仅改变解释方式，不改变二进制位模式。
inline u32 as_u32(s32 value) {
    return static_cast<u32>(value);
}

inline s32 as_s32(u32 value) {
    return static_cast<s32>(value);
}

// 把 32 位机器码拆成 DecodedInst，非法/暂不支持的 opcode 标为 Invalid。
DecodedInst decode(u32 inst);

// 生成便于调试输出的简易反汇编文本。
std::string disassemble(const DecodedInst &inst);

} // namespace riscv_sim

#endif
