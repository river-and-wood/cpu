#include "common.hpp"

#include <sstream>

namespace riscv_sim {

DecodedInst decode(u32 inst) {
    DecodedInst d;
    // 所有格式共享的字段先统一拆出；不同格式再补立即数和 kind。
    // RISC-V 的 rd/rs1/rs2/funct3/funct7 在大多数格式中位置固定，
    // 即使某些格式不用这些字段，先拆出来也不会影响后续执行。
    d.raw = inst;
    d.opcode = static_cast<u8>(inst & 0x7fu);
    d.rd = static_cast<u8>((inst >> 7) & 0x1fu);
    d.funct3 = static_cast<u8>((inst >> 12) & 0x7u);
    d.rs1 = static_cast<u8>((inst >> 15) & 0x1fu);
    d.rs2 = static_cast<u8>((inst >> 20) & 0x1fu);
    d.funct7 = static_cast<u8>((inst >> 25) & 0x7fu);
    d.imm = 0;
    d.kind = OpcodeKind::Invalid;

    switch (d.opcode) {
    case 0x37:
        // U-type: imm[31:12] 直接放到高 20 位。
        // LUI 的语义是 rd = imm，不需要读取任何源寄存器。
        d.kind = OpcodeKind::Lui;
        d.imm = static_cast<s32>(inst & 0xfffff000u);
        break;
    case 0x17:
        // AUIPC 与 LUI 编码格式相同，但执行语义是 rd = pc + imm。
        d.kind = OpcodeKind::Auipc;
        d.imm = static_cast<s32>(inst & 0xfffff000u);
        break;
    case 0x6f:
        // J-type 立即数字段在机器码中不连续，这里按 RISC-V 规范重新拼接。
        // 拼接后最低位仍为 0，表示跳转目标至少 2 字节对齐。
        d.kind = OpcodeKind::Jal;
        d.imm = sign_extend(((inst >> 31) & 0x1u) << 20 |
                            ((inst >> 12) & 0xffu) << 12 |
                            ((inst >> 20) & 0x1u) << 11 |
                            ((inst >> 21) & 0x3ffu) << 1, 21);
        break;
    case 0x67:
        // JALR 是 I-type 立即数，目标地址由 rs1 + imm 计算，
        // 执行阶段还会清掉 bit0：target & ~1。
        d.kind = OpcodeKind::Jalr;
        d.imm = sign_extend((inst >> 20) & 0xfffu, 12);
        break;
    case 0x63:
        // B-type 分支立即数最低位恒为 0，所以编码中从 bit1 开始保存。
        // bit31 是符号位；bit7 是 imm[11]，容易和 rd 字段混淆。
        d.kind = OpcodeKind::Branch;
        d.imm = sign_extend(((inst >> 31) & 0x1u) << 12 |
                            ((inst >> 7) & 0x1u) << 11 |
                            ((inst >> 25) & 0x3fu) << 5 |
                            ((inst >> 8) & 0xfu) << 1, 13);
        break;
    case 0x03:
        // Load 也是 I-type，funct3 决定 LB/LH/LW/LBU/LHU。
        d.kind = OpcodeKind::Load;
        d.imm = sign_extend((inst >> 20) & 0xfffu, 12);
        break;
    case 0x23:
        // S-type Store 立即数拆在 funct7 和 rd 两个位置。
        // Store 没有 rd，原来 rd 所在的 bit[11:7] 被用作 imm[4:0]。
        d.kind = OpcodeKind::Store;
        d.imm = sign_extend(((inst >> 25) & 0x7fu) << 5 |
                            ((inst >> 7) & 0x1fu), 12);
        break;
    case 0x13:
        // OP-IMM 包括 addi/slti/xori/ori/andi 和立即数移位。
        d.kind = OpcodeKind::OpImm;
        d.imm = sign_extend((inst >> 20) & 0xfffu, 12);
        break;
    case 0x33:
        // OP 是 R-type，具体 ADD/SUB/MUL/DIV 等由 funct3/funct7 决定。
        d.kind = OpcodeKind::Op;
        break;
    case 0x73:
        // 当前项目只真正支持 ecall，其机器码是 0x00000073。
        d.kind = OpcodeKind::System;
        break;
    default:
        break;
    }
    return d;
}

std::string disassemble(const DecodedInst &inst) {
    std::ostringstream out;
    // 这里不是完整反汇编器，只输出足够定位调试问题的字段。
    // 反汇编文本主要用于错误信息和 trace，因此保留 funct3/funct7 这种原始字段。
    switch (inst.kind) {
    case OpcodeKind::Lui:
        out << "lui x" << unsigned(inst.rd) << "," << inst.imm;
        break;
    case OpcodeKind::Auipc:
        out << "auipc x" << unsigned(inst.rd) << "," << inst.imm;
        break;
    case OpcodeKind::Jal:
        out << "jal x" << unsigned(inst.rd) << "," << inst.imm;
        break;
    case OpcodeKind::Jalr:
        out << "jalr x" << unsigned(inst.rd) << "," << inst.imm
            << "(x" << unsigned(inst.rs1) << ")";
        break;
    case OpcodeKind::Branch:
        out << "branch f3=" << unsigned(inst.funct3) << " x"
            << unsigned(inst.rs1) << ",x" << unsigned(inst.rs2)
            << "," << inst.imm;
        break;
    case OpcodeKind::Load:
        out << "load f3=" << unsigned(inst.funct3) << " x"
            << unsigned(inst.rd) << "," << inst.imm
            << "(x" << unsigned(inst.rs1) << ")";
        break;
    case OpcodeKind::Store:
        out << "store f3=" << unsigned(inst.funct3) << " x"
            << unsigned(inst.rs2) << "," << inst.imm
            << "(x" << unsigned(inst.rs1) << ")";
        break;
    case OpcodeKind::OpImm:
        out << "opimm f3=" << unsigned(inst.funct3) << " x"
            << unsigned(inst.rd) << ",x" << unsigned(inst.rs1)
            << "," << inst.imm;
        break;
    case OpcodeKind::Op:
        out << "op f3=" << unsigned(inst.funct3) << " f7="
            << unsigned(inst.funct7) << " x" << unsigned(inst.rd)
            << ",x" << unsigned(inst.rs1) << ",x" << unsigned(inst.rs2);
        break;
    case OpcodeKind::System:
        out << "system";
        break;
    default:
        out << "invalid";
        break;
    }
    return out.str();
}

} // namespace riscv_sim
