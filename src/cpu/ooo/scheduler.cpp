#include "ooo_cpu.hpp"
#include "ooo_internal.hpp"
#include <cstddef>

namespace riscv_sim {

using namespace ooo_detail;

// scheduler.cpp 负责后端调度：保留站容量检查、ready 指令选择、执行槽推进。
// 调度策略保持教学模型的简单性：各保留站按队列顺序挑选第一条 ready 指令。
bool OoOCpu::rs_full(UnitKind unit) const {
    // 前端分配前调用该函数，判断目标类型的保留站是否还有空位。
    if (unit == UnitKind::Alu) return alu_rs_.size() >= ALU_RS_CAP;
    if (unit == UnitKind::LoadStore) return ls_rs_.size() >= LS_RS_CAP;
    return branch_rs_.size() >= BR_RS_CAP;
}
bool OoOCpu::enqueue_rs(const MicroOp &op) {
    // 按功能单元类型把微操作送入对应保留站；保留站内部保持分配顺序。
    if (op.unit == UnitKind::Alu) {
        if (alu_rs_.size() >= ALU_RS_CAP) return false;
        alu_rs_.push_back(op);
        return true;
    }
    if (op.unit == UnitKind::LoadStore) {
        if (ls_rs_.size() >= LS_RS_CAP) return false;
        ls_rs_.push_back(op);
        return true;
    }
    if (branch_rs_.size() >= BR_RS_CAP) return false;
    branch_rs_.push_back(op);
    return true;
}
bool OoOCpu::src_ready(const MicroOp &op) const {
    // 一条微操作必须两个 Operand 都 ready 才能离开保留站进入执行槽。
    return op.src1.ready && op.src2.ready;
}
unsigned OoOCpu::latency(const MicroOp &op) const {
    // 教学模型用固定延迟近似不同功能单元；不模拟流水化乘法器/除法器内部细节。
    if (op.unit == UnitKind::LoadStore) return 2;
    if (op.unit == UnitKind::Branch) return 1;
    if (op.inst.kind == OpcodeKind::Op && op.inst.funct7 == 0x01 && op.inst.funct3 == 0x0) return 3;  // MUL
    if (op.inst.kind == OpcodeKind::Op && op.inst.funct7 == 0x01 && op.inst.funct3 == 0x4) return 35; // DIV
    return 1;
}
void OoOCpu::start_ready_ops() {
    // ALU 有两个执行槽，按保留站顺序选择已经就绪的微操作。
    // 每个空闲 ALU 槽最多取走一条 ready 微操作，所以两个 ALU 最多同周期启动两条。
    for (unsigned fu = 0; fu < 2; ++fu) {
        if (alu_fu_[fu].busy) continue;
        for (std::size_t i = 0; i < alu_rs_.size(); ++i) {
            if (!src_ready(alu_rs_[i])) continue;
            alu_fu_[fu].busy = true;
            alu_fu_[fu].remain = latency(alu_rs_[i]);
            alu_fu_[fu].op = alu_rs_[i];
            if (trace_enabled_) trace_start(alu_fu_[fu], fu);
            // 微操作一旦进入执行槽，就从保留站删除，避免被重复发射。
            erase_started(&alu_rs_, i);
            break;
        }
    }

    if (!ls_fu_.busy) {
        // Load/Store 共享一个执行槽，所以同一周期最多启动一个访存类微操作。
        for (std::size_t i = 0; i < ls_rs_.size(); ++i) {
            if (!src_ready(ls_rs_[i])) continue;
            // 简化的内存相关处理：Load 不越过任何更老 Store。
            if (ls_rs_[i].inst.kind == OpcodeKind::Load && has_older_store(ls_rs_[i].dst_tag)) continue;
            ls_fu_.busy = true;
            ls_fu_.remain = latency(ls_rs_[i]);
            ls_fu_.op = ls_rs_[i];
            if (trace_enabled_) trace_start(ls_fu_, 2);
            erase_started(&ls_rs_, i);
            break;
        }
    }

    if (!branch_fu_.busy) {
        // 分支单元独立于 ALU/LS，可以与它们并行执行。
        for (std::size_t i = 0; i < branch_rs_.size(); ++i) {
            if (!src_ready(branch_rs_[i])) continue;
            branch_fu_.busy = true;
            branch_fu_.remain = latency(branch_rs_[i]);
            branch_fu_.op = branch_rs_[i];
            if (trace_enabled_) trace_start(branch_fu_, 3);
            erase_started(&branch_rs_, i);
            break;
        }
    }
}
void OoOCpu::erase_started(std::vector<MicroOp> *rs, std::size_t index) {
    // vector::erase 需要迭代器；index 转 ptrdiff_t 是为了匹配迭代器偏移类型。
    rs->erase(rs->begin() + static_cast<std::ptrdiff_t>(index));
}
bool OoOCpu::complete_exec_slots(std::vector<WriteBack> *wbs, Redirect *redirect, std::string *error) {
    // 执行槽在这里递减剩余延迟；延迟归零时生成写回事件和可能的控制重定向。
    // slots 数组把四个执行槽统一遍历，减少重复代码。
    ExecSlot *slots[] = { &alu_fu_[0], &alu_fu_[1], &ls_fu_, &branch_fu_ };
    for (unsigned i = 0; i < 4; ++i) {
        ExecSlot *slot = slots[i];
        if (!slot->busy) continue;
        if (slot->remain > 0) --slot->remain;
        if (slot->remain != 0) continue;

        WriteBack wb;
        Redirect local_redirect;
        // 同一周期多个控制指令完成时，保留最老 tag 的重定向，避免错误路径覆盖正确路径。
        if (!execute_result(slot->op, &wb, &local_redirect, error)) return false;
        if (wb.valid) wbs->push_back(wb);
        if (trace_enabled_ && wb.valid) trace_writeback(wb);
        if (local_redirect.valid && (!redirect->valid || local_redirect.tag < redirect->tag))
            *redirect = local_redirect;
        slot->busy = false;
    }
    return true;
}
bool OoOCpu::has_older_store(u32 tag) const {
    // 从 ROB 头向后扫描，在当前 Load 之前看到 Store 就保守等待。
    // 项目没有实现完整 LSQ/地址预测，因此 Load 不越过更老 Store 是更安全的简化。
    for (std::size_t i = 0; i < rob_.size(); ++i) {
        if (rob_[i].tag == tag) return false;
        if (rob_[i].is_store) return true;
    }
    return false;
}

} // namespace riscv_sim
