#include "cpu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace riscv_sim {

Cpu::Cpu(Memory &memory) : mem_(memory), pc_(0), cycles_(0) {
    // regs_ 保存架构寄存器 x0..x31；构造时全部清零。
    for (unsigned i = 0; i < 32; ++i) regs_[i] = 0;
}

void Cpu::reset(u32 pc) {
    // reset 不清空 Memory，便于先加载程序再重置 CPU 从指定 PC 开始执行。
    for (unsigned i = 0; i < 32; ++i) regs_[i] = 0;
    pc_ = pc;
    cycles_ = 0;
    mem_.set_cycles(0);
}

bool Cpu::step(std::string *error) {
    if (mem_.stopped()) return false;

    // 顺序模型一次只处理一条指令，next_pc 默认指向顺序下一条。
    // 如果遇到跳转/分支，后面的 case 会覆盖 next_pc。
    const u32 inst_word = mem_.read32(pc_);
    const DecodedInst inst = decode(inst_word);
    u32 next_pc = pc_ + 4u;
    bool valid = true;

    switch (inst.kind) {
    case OpcodeKind::Lui:
        // LUI/AUIPC/JAL/JALR 都可能写 rd，write_reg 会自动保护 x0。
        write_reg(inst.rd, static_cast<u32>(inst.imm));
        break;
    case OpcodeKind::Auipc:
        // AUIPC 常用于位置无关代码：把当前 PC 与高 20 位立即数相加。
        write_reg(inst.rd, pc_ + static_cast<u32>(inst.imm));
        break;
    case OpcodeKind::Jal:
        // JAL 同时完成两件事：rd 保存返回地址，PC 跳到 pc + imm。
        write_reg(inst.rd, pc_ + 4u);
        next_pc = pc_ + static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Jalr:
        // JALR 的目标来自寄存器，最低位按规范清零。
        write_reg(inst.rd, pc_ + 4u);
        next_pc = (regs_[inst.rs1] + static_cast<u32>(inst.imm)) & ~1u;
        break;
    case OpcodeKind::Branch: {
        const u32 x = regs_[inst.rs1];
        const u32 y = regs_[inst.rs2];
        bool take = false;
        // funct3 决定具体比较方式：有符号比较需要转成 s32。
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
        // Load/Store 地址统一是 base(rs1) + sign-extended immediate。
        const u32 addr = regs_[inst.rs1] + static_cast<u32>(inst.imm);
        // Load 的 funct3 同时决定访问宽度和是否符号扩展。
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
        // Store 只写内存，不修改寄存器。
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
        // 移位立即数只使用低 5 位，因为 RV32 的移位范围是 0..31。
        const u32 shamt = (inst.raw >> 20) & 0x1fu;
        // I-type 算术/逻辑指令，移位类需要额外检查 funct7。
        switch (inst.funct3) {
        case 0x0:
            write_reg(inst.rd, x + static_cast<u32>(inst.imm));
            break;
        case 0x2:
            // SLTI 是有符号比较，比较前要把 x 按 s32 解释。
            write_reg(inst.rd, as_s32(x) < inst.imm ? 1u : 0u);
            break;
        case 0x3:
            // SLTIU 是无符号比较，立即数虽然符号扩展，但比较按 u32 位模式进行。
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
        // R-type 算术/逻辑指令；funct7=0x01 表示当前支持的 M 扩展 MUL/DIV。
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
                // DIV 按 RISC-V M 扩展处理除零和有符号溢出的特殊返回值。
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
            // 本项目把 ecall 约定为向 IO_TIMER 写入，从而触发模拟器停机。
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
        // error 是可选输出参数；调用者传空指针时只返回 false。
        if (error) {
            std::ostringstream out;
            out << "unsupported instruction at pc=0x" << std::hex << pc_
                << " inst=0x" << inst_word << " (" << disassemble(inst) << ")";
            *error = out.str();
        }
        return false;
    }

    pc_ = next_pc;
    // RISC-V x0 永远为 0；这里再次归零防止实现疏漏。
    regs_[0] = 0;
    ++cycles_;
    mem_.set_cycles(cycles_);
    return !mem_.stopped();
}

bool Cpu::run(u64 max_cycles, std::string *error) {
    // run 是 step 的简单循环包装：正常停机依赖 Memory::stopped()。
    for (u64 i = 0; i < max_cycles; ++i) {
        if (!step(error)) return mem_.stopped();
    }
    if (error) *error = "cycle limit reached";
    return false;
}

void Cpu::dump_registers(std::ostream &out) const {
    // iomanip 控制十六进制、宽度和填充字符，使寄存器输出固定为 8 位。
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
    // x0 是硬连线 0，任何写 x0 的指令都应该被忽略。
    if (rd != 0) regs_[rd] = value;
}

} // namespace riscv_sim
