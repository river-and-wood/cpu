#include "ooo_cpu.hpp"
#include "ooo_internal.hpp"
#include <algorithm>
#include <iostream>

namespace riscv_sim {

using namespace ooo_detail;

// rob.cpp 负责公共数据总线、ROB 状态更新、按序提交，以及分支 flush 后的状态恢复。
// 乱序执行的结果只有在 ROB 头部提交时才会修改架构寄存器或内存。
const OoOCpu::RobEntry *OoOCpu::find_rob(u32 tag) const {
    // ROB 容量很小，线性查找足够清晰；真实硬件会用数组索引/指针。
    for (std::size_t i = 0; i < rob_.size(); ++i) {
        if (rob_[i].tag == tag) return &rob_[i];
    }
    return 0;
}
OoOCpu::RobEntry *OoOCpu::find_rob(u32 tag) {
    for (std::size_t i = 0; i < rob_.size(); ++i) {
        if (rob_[i].tag == tag) return &rob_[i];
    }
    return 0;
}
void OoOCpu::broadcast(const std::vector<WriteBack> &wbs) {
    // 一个周期可能有多个执行槽完成，因此 wbs 是数组而不是单个 WriteBack。
    for (std::size_t i = 0; i < wbs.size(); ++i) {
        const WriteBack &wb = wbs[i];
        // 写回同时更新 ROB，并唤醒所有保留站中等待该 tag 的操作数。
        apply_writeback_to_rob(wb);
        for (std::size_t j = 0; j < alu_rs_.size(); ++j) {
            update_waiting_operand(&alu_rs_[j].src1, wb);
            update_waiting_operand(&alu_rs_[j].src2, wb);
        }
        for (std::size_t j = 0; j < ls_rs_.size(); ++j) {
            update_waiting_operand(&ls_rs_[j].src1, wb);
            update_waiting_operand(&ls_rs_[j].src2, wb);
        }
        for (std::size_t j = 0; j < branch_rs_.size(); ++j) {
            update_waiting_operand(&branch_rs_[j].src1, wb);
            update_waiting_operand(&branch_rs_[j].src2, wb);
        }
    }
}
void OoOCpu::apply_writeback_to_rob(const WriteBack &wb) {
    if (!wb.valid) return;
    RobEntry *entry = find_rob(wb.tag);
    if (!entry) return;
    // ready=true 表示 ROB 头部轮到它时可以提交；但现在不一定已经在队头。
    entry->ready = true;
    entry->has_value = wb.has_value;
    entry->value = wb.value;
    entry->actual_pc = entry->predicted_pc;
    if (wb.is_store) {
        // Store 的地址和值先放进 ROB，真正写内存要等 commit_one。
        entry->is_store = true;
        entry->store_addr = wb.store_addr;
        entry->store_value = wb.store_value;
    }
}
bool OoOCpu::commit_one(std::string *error) {
    // 每周期最多提交 ROB 头部一条；头部未 ready 时，后面即使 ready 也不能越过提交。
    if (rob_.empty() || !rob_.front().ready) return true;

    RobEntry entry = rob_.front();
    if (entry.is_store) {
        // Store 在 ROB 头部提交时才写内存，保证异常/分支 flush 不留下副作用。
        if (!store_value(mem_, entry.inst, entry.store_addr, entry.store_value)) {
            if (error) *error = "unsupported store at commit";
            return false;
        }
        ++stats_.store_commits;
    }

    if (entry.has_value && entry.rd != 0) {
        // 只有当前 ROB tag 仍是该寄存器最新写者时，才释放重命名锁。
        // 如果后面更年轻指令已经重命名同一 rd，regs_[rd].tag 会是更大的 tag，不能清零。
        regs_[entry.rd].value = entry.value;
        if (regs_[entry.rd].tag == entry.tag) regs_[entry.rd].tag = 0;
    }

    if (trace_enabled_) trace_commit(entry);

    rob_.erase(rob_.begin());
    ++stats_.committed;

    if (entry.is_ecall) {
        // ecall 提交说明它之前的所有指令都已经提交，可以安全结束模拟。
        halt_when_empty_ = false;
        fetch_stalled_ = false;
        if (trace_enabled_ && !trace_json_) std::cout << "  停机: ecall 按序提交\n";
        mem_.write8(IO_TIMER, 0);
    }
    return true;
}
void OoOCpu::flush_younger_than(u32 tag) {
    // 清除所有 tag 更大的推测指令，包括保留站、正在执行的 FU 和 ROB。
    // std::remove_if 先把保留元素移到前面，erase 再真正缩短 vector。
    alu_rs_.erase(std::remove_if(alu_rs_.begin(), alu_rs_.end(),
                                 [tag](const MicroOp &op) { return op.dst_tag > tag; }),
                  alu_rs_.end());
    ls_rs_.erase(std::remove_if(ls_rs_.begin(), ls_rs_.end(),
                                [tag](const MicroOp &op) { return op.dst_tag > tag; }),
                 ls_rs_.end());
    branch_rs_.erase(std::remove_if(branch_rs_.begin(), branch_rs_.end(),
                                    [tag](const MicroOp &op) { return op.dst_tag > tag; }),
                     branch_rs_.end());

    if (alu_fu_[0].busy && alu_fu_[0].op.dst_tag > tag) alu_fu_[0] = ExecSlot();
    if (alu_fu_[1].busy && alu_fu_[1].op.dst_tag > tag) alu_fu_[1] = ExecSlot();
    if (ls_fu_.busy && ls_fu_.op.dst_tag > tag) ls_fu_ = ExecSlot();
    if (branch_fu_.busy && branch_fu_.op.dst_tag > tag) branch_fu_ = ExecSlot();

    rob_.erase(std::remove_if(rob_.begin(), rob_.end(),
                              [tag](const RobEntry &entry) { return entry.tag > tag; }),
               rob_.end());
    // 删除年轻指令后，有些寄存器 tag 可能指向已删除 ROB 项，必须重建。
    rebuild_rename_state();
    halt_when_empty_ = false;
    for (std::size_t i = 0; i < rob_.size(); ++i) {
        if (rob_[i].is_ecall) halt_when_empty_ = true;
    }
}
void OoOCpu::rebuild_rename_state() {
    // flush 后重命名表可能指向已删除的 ROB 项，因此从剩余 ROB 顺序重建。
    for (unsigned i = 0; i < 32; ++i) regs_[i].tag = 0;
    for (std::size_t i = 0; i < rob_.size(); ++i) {
        // 顺序扫描 ROB，后面的同 rd 写者会覆盖前面的 tag，得到“最新未提交写者”。
        if (writes_register(rob_[i].inst)) regs_[rob_[i].rd].tag = rob_[i].tag;
    }
}
void OoOCpu::update_waiting_operand(Operand *operand, const WriteBack &wb) {
    // 公共数据总线唤醒：tag 匹配且广播携带寄存器值时，操作数变为 ready。
    // Store/Branch 的写回没有 has_value，不应该唤醒等待寄存器值的操作数。
    if (!wb.valid || !wb.has_value || operand->ready || operand->tag != wb.tag) return;
    operand->ready = true;
    operand->value = wb.value;
    operand->tag = 0;
}

} // namespace riscv_sim
