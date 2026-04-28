#include "cpu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace riscv_sim {

Cpu::Cpu(Memory &memory) : mem_(memory), pc_(0), cycles_(0) {
    for (unsigned i = 0; i < 32; ++i) regs_[i] = 0;
}

void Cpu::reset(u32 pc) {
    for (unsigned i = 0; i < 32; ++i) regs_[i] = 0;
    pc_ = pc;
    cycles_ = 0;
    mem_.set_cycles(0);
}

bool Cpu::step(std::string *error) {
    if (mem_.stopped()) return false;

    const u32 inst_word = mem_.read32(pc_);
    const DecodedInst inst = decode(inst_word);
    u32 next_pc = pc_ + 4u;
    bool valid = true;

    switch (inst.kind) {
    case OpcodeKind::Lui:
        write_reg(inst.rd, static_cast<u32>(inst.imm));
        break;
    case OpcodeKind::Auipc:
        write_reg(inst.rd, pc_ + static_cast<u32>(inst.imm));
        break;
    case OpcodeKind::Jal:
        write_reg(inst.rd, pc_ + 4u);
        next_pc = pc_ + static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Jalr:
        write_reg(inst.rd, pc_ + 4u);
        next_pc = (regs_[inst.rs1] + static_cast<u32>(inst.imm)) & ~1u;
        break;
    case OpcodeKind::Branch: {
        const u32 x = regs_[inst.rs1];
        const u32 y = regs_[inst.rs2];
        bool take = false;
        switch (inst.funct3) {
        case 0x0: take = (x == y); break;
        case 0x1: take = (x != y); break;
        case 0x4: take = (as_s32(x) < as_s32(y)); break;
        case 0x5: take = (as_s32(x) >= as_s32(y)); break;
        case 0x6: take = (x < y); break;
        case 0x7: take = (x >= y); break;
        default: valid = false; break;
        }
        if (take) next_pc = pc_ + static_cast<u32>(inst.imm);
        break;
    }
    case OpcodeKind::Load: {
        const u32 addr = regs_[inst.rs1] + static_cast<u32>(inst.imm);
        switch (inst.funct3) {
        case 0x0:
            write_reg(inst.rd, static_cast<u32>(sign_extend(mem_.read8(addr), 8)));
            break;
        case 0x1:
            write_reg(inst.rd, static_cast<u32>(sign_extend(mem_.read16(addr), 16)));
            break;
        case 0x2:
            write_reg(inst.rd, mem_.read32(addr));
            break;
        case 0x4:
            write_reg(inst.rd, mem_.read8(addr));
            break;
        case 0x5:
            write_reg(inst.rd, mem_.read16(addr));
            break;
        default:
            valid = false;
            break;
        }
        break;
    }
    case OpcodeKind::Store: {
        const u32 addr = regs_[inst.rs1] + static_cast<u32>(inst.imm);
        const u32 value = regs_[inst.rs2];
        switch (inst.funct3) {
        case 0x0: mem_.write8(addr, static_cast<u8>(value)); break;
        case 0x1: mem_.write16(addr, static_cast<u16>(value)); break;
        case 0x2: mem_.write32(addr, value); break;
        default: valid = false; break;
        }
        break;
    }
    case OpcodeKind::OpImm: {
        const u32 x = regs_[inst.rs1];
        const u32 shamt = (inst.raw >> 20) & 0x1fu;
        switch (inst.funct3) {
        case 0x0:
            write_reg(inst.rd, x + static_cast<u32>(inst.imm));
            break;
        case 0x2:
            write_reg(inst.rd, as_s32(x) < inst.imm ? 1u : 0u);
            break;
        case 0x3:
            write_reg(inst.rd, x < static_cast<u32>(inst.imm) ? 1u : 0u);
            break;
        case 0x4:
            write_reg(inst.rd, x ^ static_cast<u32>(inst.imm));
            break;
        case 0x6:
            write_reg(inst.rd, x | static_cast<u32>(inst.imm));
            break;
        case 0x7:
            write_reg(inst.rd, x & static_cast<u32>(inst.imm));
            break;
        case 0x1:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x << shamt);
            else valid = false;
            break;
        case 0x5:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x >> shamt);
            else if (inst.funct7 == 0x20) write_reg(inst.rd, static_cast<u32>(as_s32(x) >> shamt));
            else valid = false;
            break;
        default:
            valid = false;
            break;
        }
        break;
    }
    case OpcodeKind::Op: {
        const u32 x = regs_[inst.rs1];
        const u32 y = regs_[inst.rs2];
        const u32 shamt = y & 0x1fu;
        switch (inst.funct3) {
        case 0x0:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x + y);
            else if (inst.funct7 == 0x20) write_reg(inst.rd, x - y);
            else if (inst.funct7 == 0x01) write_reg(inst.rd, x * y);
            else valid = false;
            break;
        case 0x1:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x << shamt);
            else valid = false;
            break;
        case 0x2:
            if (inst.funct7 == 0x00) write_reg(inst.rd, as_s32(x) < as_s32(y) ? 1u : 0u);
            else valid = false;
            break;
        case 0x3:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x < y ? 1u : 0u);
            else valid = false;
            break;
        case 0x4:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x ^ y);
            else if (inst.funct7 == 0x01) {
                if (y == 0) write_reg(inst.rd, 0xffffffffu);
                else if (x == 0x80000000u && y == 0xffffffffu) write_reg(inst.rd, x);
                else write_reg(inst.rd, static_cast<u32>(as_s32(x) / as_s32(y)));
            } else valid = false;
            break;
        case 0x5:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x >> shamt);
            else if (inst.funct7 == 0x20) write_reg(inst.rd, static_cast<u32>(as_s32(x) >> shamt));
            else valid = false;
            break;
        case 0x6:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x | y);
            else valid = false;
            break;
        case 0x7:
            if (inst.funct7 == 0x00) write_reg(inst.rd, x & y);
            else valid = false;
            break;
        default:
            valid = false;
            break;
        }
        break;
    }
    case OpcodeKind::System:
        if (inst.raw == 0x00000073u) {
            mem_.write8(IO_TIMER, 0);
        } else {
            valid = false;
        }
        break;
    default:
        valid = false;
        break;
    }

    if (!valid) {
        if (error) {
            std::ostringstream out;
            out << "unsupported instruction at pc=0x" << std::hex << pc_
                << " inst=0x" << inst_word << " (" << disassemble(inst) << ")";
            *error = out.str();
        }
        return false;
    }

    pc_ = next_pc;
    regs_[0] = 0;
    ++cycles_;
    mem_.set_cycles(cycles_);
    return !mem_.stopped();
}

bool Cpu::run(u64 max_cycles, std::string *error) {
    for (u64 i = 0; i < max_cycles; ++i) {
        if (!step(error)) return mem_.stopped();
    }
    if (error) *error = "cycle limit reached";
    return false;
}

void Cpu::dump_registers(std::ostream &out) const {
    out << std::hex << std::setfill('0');
    for (unsigned i = 0; i < 32; ++i) {
        out << "x" << std::dec << std::setw(2) << std::setfill(' ') << i
            << std::setfill('0') << "=0x" << std::hex << std::setw(8)
            << regs_[i] << ((i % 4 == 3) ? '\n' : ' ');
    }
    out << std::dec << std::setfill(' ');
}

u32 Cpu::reg(unsigned index) const {
    return index < 32 ? regs_[index] : 0;
}

u32 Cpu::pc() const { return pc_; }
u64 Cpu::cycles() const { return cycles_; }

void Cpu::write_reg(u8 rd, u32 value) {
    if (rd != 0) regs_[rd] = value;
}

} // namespace riscv_sim
