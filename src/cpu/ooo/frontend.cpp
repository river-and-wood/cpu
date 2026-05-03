#include "ooo_cpu.hpp"
#include "ooo_internal.hpp"
#include <iostream>
#include <sstream>

namespace riscv_sim {

using namespace ooo_detail;

// frontend.cpp 负责取指、解码、寄存器重命名，以及把新指令分配进 ROB/保留站。
// 如果资源不足，本周期只记录 stall，PC 不前进，下一周期重试同一条指令。
bool OoOCpu::allocate_one(std::string *error) {
    if (rob_.size() >= ROB_CAP) {
        // ROB 满时不能继续给新指令分配 tag，否则按序提交窗口会溢出。
        ++stats_.rob_stalls;
        if (trace_enabled_) trace_stall("ROB 已满，分配暂停并重试当前 PC");
        return true;
    }

    const u32 inst_word = icache_.fetch(mem_, pc_);
    const DecodedInst inst = decode(inst_word);

    if (inst.kind == OpcodeKind::System) {
        if (inst.raw == 0x00000073u) {
            // ecall 必须像普通指令一样进入 ROB，等待前面所有指令提交后再停机。
            // 它本身不需要执行单元，ready=true，等排到 ROB 头时 commit_one 触发停机。
            const u32 tag = next_tag_++;
            RobEntry entry;
            entry.valid = true;
            entry.ready = true;
            entry.is_ecall = true;
            entry.tag = tag;
            entry.pc = pc_;
            entry.predicted_pc = pc_ + 4u;
            entry.actual_pc = pc_ + 4u;
            entry.inst = inst;
            rob_.push_back(entry);
            ++stats_.allocated;
            halt_when_empty_ = true;
            fetch_stalled_ = true;
            // ecall 之后不再取新指令，直到它提交；这样避免停机点之后的代码进入乱序窗口。
            if (trace_enabled_) {
                if (trace_json_) {
                    std::cout << "{\"event\":\"allocate\",\"cycle\":" << cycles_
                              << ",\"pc\":" << pc_ << ",\"inst\":" << inst_word
                              << ",\"tag\":" << tag << ",\"kind\":\"ecall\"}\n";
                } else {
                    std::cout << "  取指: pc=0x" << std::hex << pc_ << " inst=0x" << inst_word
                              << std::dec << " -> ecall 入 ROB，等待按序提交\n";
                }
            }
            pc_ += 4;
            return true;
        }
        if (error) *error = "unsupported system instruction";
        return false;
    }

    MicroOp op;
    if (!decode_to_micro_op(inst, pc_, &op, error)) return false;
    if (rs_full(op.unit)) {
        // 保留站满时不推进 PC，也不分配 ROB tag，下一周期会重新尝试同一条指令。
        ++stats_.rs_stalls;
        if (trace_enabled_) trace_stall("目标保留站已满，分配暂停并重试当前 PC");
        return true; // 保留站满，当前周期停顿，下一周期重试同一 PC。
    }

    const u32 tag = next_tag_++;
    op.dst_tag = tag;
    if (inst.kind == OpcodeKind::Branch || inst.kind == OpcodeKind::Jalr) {
        op.predicted_pc = pc_ + 4u; // 静态 not-taken；JALR 先按顺序路径预测。
        ++stats_.branch_predictions;
    } else if (inst.kind == OpcodeKind::Jal) {
        // JAL 的目标由立即数直接确定，所以前端可以无条件预测为跳转目标。
        op.predicted_pc = pc_ + static_cast<u32>(inst.imm);
    } else {
        op.predicted_pc = pc_ + 4u;
    }

    if (!enqueue_rs(op)) {
        // 理论上前面 rs_full 已检查过；这里保留二次检查，避免未来改动破坏容量约束。
        ++stats_.rs_stalls;
        if (trace_enabled_) trace_stall("目标保留站已满，分配暂停并重试当前 PC");
        return true; // 保留站满，当前周期停顿，下一周期重试同一 PC。
    }

    RobEntry entry;
    // ROB 记录推测执行结果；只有 commit_one 才会修改架构寄存器/内存。
    entry.valid = true;
    entry.ready = false;
    entry.tag = tag;
    entry.rd = inst.rd;
    entry.pc = pc_;
    entry.predicted_pc = op.predicted_pc;
    entry.actual_pc = op.predicted_pc;
    entry.inst = inst;
    entry.is_store = inst.kind == OpcodeKind::Store;
    entry.is_control = inst.kind == OpcodeKind::Branch || inst.kind == OpcodeKind::Jal || inst.kind == OpcodeKind::Jalr;
    rob_.push_back(entry);
    ++stats_.allocated;

    if (trace_enabled_) trace_allocate(op);

    if (writes_register(inst)) {
        regs_[inst.rd].tag = tag; // 目标寄存器加锁：后续读者等待该 ROB tag。
    }

    pc_ = op.predicted_pc;
    return true;
}
bool OoOCpu::decode_to_micro_op(const DecodedInst &inst, u32 pc, MicroOp *out, std::string *error) {
    // 将 ISA 级 DecodedInst 转成后端微操作，并根据指令类型选择执行单元。
    MicroOp op;
    // 默认两个源操作数都 ready；真正需要源寄存器的指令会在 switch 中覆盖。
    op.valid = true;
    op.inst = inst;
    op.pc = pc;
    op.rd = inst.rd;
    op.op = static_cast<OpKind>(inst.kind);
    op.src1.ready = true;
    op.src2.ready = true;

    switch (inst.kind) {
    case OpcodeKind::Lui:
    case OpcodeKind::Auipc:
        op.unit = UnitKind::Alu;
        break;
    case OpcodeKind::Jal:
        op.unit = UnitKind::Alu;
        break;
    case OpcodeKind::Jalr:
        op.unit = UnitKind::Alu;
        // JALR 的目标地址依赖 rs1，需要通过重命名表读取或挂 tag。
        op.src1 = read_operand(inst.rs1);
        break;
    case OpcodeKind::Branch:
        op.unit = UnitKind::Branch;
        // 分支比较需要两个源寄存器；如果任意一个还没产生，就在 Branch RS 中等待。
        op.src1 = read_operand(inst.rs1);
        op.src2 = read_operand(inst.rs2);
        break;
    case OpcodeKind::Load:
        op.unit = UnitKind::LoadStore;
        // Load 只依赖地址基址 rs1；立即数已经在 DecodedInst::imm 中。
        op.src1 = read_operand(inst.rs1);
        break;
    case OpcodeKind::Store:
        op.unit = UnitKind::LoadStore;
        // Store 同时依赖地址基址 rs1 和待写数据 rs2。
        op.src1 = read_operand(inst.rs1);
        op.src2 = read_operand(inst.rs2);
        break;
    case OpcodeKind::OpImm:
        op.unit = UnitKind::Alu;
        // OP-IMM 只读 rs1，立即数不需要作为 Operand 追踪依赖。
        op.src1 = read_operand(inst.rs1);
        break;
    case OpcodeKind::Op:
        op.unit = UnitKind::Alu;
        op.src1 = read_operand(inst.rs1);
        op.src2 = read_operand(inst.rs2);
        break;
    default:
        if (error) {
            std::ostringstream out_msg;
            out_msg << "unsupported instruction at pc=0x" << std::hex << pc
                    << " inst=0x" << inst.raw << " (" << disassemble(inst) << ")";
            *error = out_msg.str();
        }
        return false;
    }

    *out = op;
    return true;
}
OoOCpu::Operand OoOCpu::read_operand(u8 reg) const {
    // 读取源寄存器时同时查询重命名 tag，形成保留站中的 ready/value/tag 三元组。
    Operand op;
    if (reg == 0) {
        // x0 永远就绪且值为 0，不参与重命名等待。
        op.ready = true;
        op.value = 0;
        op.tag = 0;
        return op;
    }
    if (regs_[reg].tag == 0) {
        // 没有未提交写者，直接读取架构寄存器值。
        op.ready = true;
        op.value = regs_[reg].value;
        op.tag = 0;
    } else {
        // 有未提交写者：若 ROB 已经 ready 可以旁路读取，否则记录等待 tag。
        // 这里的旁路读取避免“结果已经写回 ROB 但还没提交”时产生不必要等待。
        const RobEntry *entry = find_rob(regs_[reg].tag);
        if (entry && entry->ready && entry->has_value) {
            op.ready = true;
            op.value = entry->value;
            op.tag = 0;
        } else {
            op.ready = false;
            op.value = 0;
            op.tag = regs_[reg].tag;
        }
    }
    return op;
}

} // namespace riscv_sim
