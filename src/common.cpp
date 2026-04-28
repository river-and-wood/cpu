#include "common.hpp"

#include <sstream>

namespace riscv_sim {

DecodedInst decode(u32 inst) {
    DecodedInst d;
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
        d.kind = OpcodeKind::Lui;
        d.imm = static_cast<s32>(inst & 0xfffff000u);
        break;
    case 0x17:
        d.kind = OpcodeKind::Auipc;
        d.imm = static_cast<s32>(inst & 0xfffff000u);
        break;
    case 0x6f:
        d.kind = OpcodeKind::Jal;
        d.imm = sign_extend(((inst >> 31) & 0x1u) << 20 |
                            ((inst >> 12) & 0xffu) << 12 |
                            ((inst >> 20) & 0x1u) << 11 |
                            ((inst >> 21) & 0x3ffu) << 1, 21);
        break;
    case 0x67:
        d.kind = OpcodeKind::Jalr;
        d.imm = sign_extend((inst >> 20) & 0xfffu, 12);
        break;
    case 0x63:
        d.kind = OpcodeKind::Branch;
        d.imm = sign_extend(((inst >> 31) & 0x1u) << 12 |
                            ((inst >> 7) & 0x1u) << 11 |
                            ((inst >> 25) & 0x3fu) << 5 |
                            ((inst >> 8) & 0xfu) << 1, 13);
        break;
    case 0x03:
        d.kind = OpcodeKind::Load;
        d.imm = sign_extend((inst >> 20) & 0xfffu, 12);
        break;
    case 0x23:
        d.kind = OpcodeKind::Store;
        d.imm = sign_extend(((inst >> 25) & 0x7fu) << 5 |
                            ((inst >> 7) & 0x1fu), 12);
        break;
    case 0x13:
        d.kind = OpcodeKind::OpImm;
        d.imm = sign_extend((inst >> 20) & 0xfffu, 12);
        break;
    case 0x33:
        d.kind = OpcodeKind::Op;
        break;
    case 0x73:
        d.kind = OpcodeKind::System;
        break;
    default:
        break;
    }
    return d;
}

std::string disassemble(const DecodedInst &inst) {
    std::ostringstream out;
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
