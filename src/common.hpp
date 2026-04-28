#ifndef CPP_SIM_COMMON_HPP
#define CPP_SIM_COMMON_HPP

#include <cstdint>
#include <string>

namespace riscv_sim {

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;
typedef std::int32_t s32;

static const u32 RAM_SIZE = 128u * 1024u;
static const u32 IO_TIMER = 0x00030004u;

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

struct DecodedInst {
    u32 raw;
    OpcodeKind kind;
    u8 opcode;
    u8 rd;
    u8 rs1;
    u8 rs2;
    u8 funct3;
    u8 funct7;
    s32 imm;
};

inline s32 sign_extend(u32 value, unsigned bits) {
    const u32 mask = 1u << (bits - 1u);
    return static_cast<s32>((value ^ mask) - mask);
}

inline u32 as_u32(s32 value) {
    return static_cast<u32>(value);
}

inline s32 as_s32(u32 value) {
    return static_cast<s32>(value);
}

DecodedInst decode(u32 inst);
std::string disassemble(const DecodedInst &inst);

} // namespace riscv_sim

#endif
