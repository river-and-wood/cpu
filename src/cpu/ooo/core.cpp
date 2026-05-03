#include "ooo_cpu.hpp"

namespace riscv_sim {

// core.cpp 负责 OoOCpu 的生命周期和每周期流水推进。
// 这里不放具体指令语义，目的是让 step() 只表达“本周期各阶段顺序”。
OoOCpu::OoOCpu(Memory &memory) : mem_(memory) {
    // CPU 持有 Memory 的引用，所有取指、Load/Store、MMIO 都访问同一个 Memory 对象。
    clear();
}
void OoOCpu::clear() {
    // 清空所有架构状态、推测状态和性能计数器，供构造和 reset 复用。
    for (unsigned i = 0; i < 32; ++i) regs_[i] = RegSlot();
    pc_ = 0;
    cycles_ = 0;
    // tag=0 被保留为“没有依赖”，所以第一个真实 ROB tag 从 1 开始。
    next_tag_ = 1;
    fetch_stalled_ = false;
    halt_when_empty_ = false;
    trace_enabled_ = false;
    trace_json_ = false;
    alu_rs_.clear();
    ls_rs_.clear();
    branch_rs_.clear();
    rob_.clear();
    // ExecSlot() 构造一个空槽，等价于把 busy=false、remain=0、op 清空。
    alu_fu_[0] = ExecSlot();
    alu_fu_[1] = ExecSlot();
    ls_fu_ = ExecSlot();
    branch_fu_ = ExecSlot();
    stats_ = Stats();
}
void OoOCpu::reset(u32 pc) {
    clear();
    pc_ = pc;
    // reset 时清空 ICache 统计和缓存行；Memory 内容保留，方便先加载程序再 reset CPU。
    icache_.reset();
    mem_.set_cycles(0);
}
void OoOCpu::set_trace(bool enabled) {
    trace_enabled_ = enabled;
}
void OoOCpu::set_trace_json(bool enabled) {
    trace_json_ = enabled;
    if (enabled) trace_enabled_ = true;
}
bool OoOCpu::step(std::string *error) {
    if (mem_.stopped()) return false;

    if (trace_enabled_) trace_cycle_begin();

    // 1. 上周期已发射的执行单元先递减延迟并产生写回/重定向。
    // 先完成再发射，相当于本周期末尾新发射的指令不会立刻完成。
    std::vector<WriteBack> wbs;
    Redirect redirect;
    if (!complete_exec_slots(&wbs, &redirect, error)) return false;

    // 2. 公共数据总线广播结果，唤醒保留站并更新 ROB ready 状态。
    // 这个步骤模拟 CDB：完成的生产者把 tag/value 广播给所有等待者。
    broadcast(wbs);

    if (redirect.valid) {
        // 3. 分支/JALR 预测错误：清掉更年轻的推测状态，并从正确 PC 重新取指。
        ++stats_.branch_mispredictions;
        ++stats_.flushes;
        flush_younger_than(redirect.tag);
        if (trace_enabled_) {
            trace_redirect(redirect.pc);
            trace_flush(redirect.tag, redirect.pc);
        }
        pc_ = redirect.pc;
        fetch_stalled_ = false;
    }

    if (!commit_one(error)) return false;
    if (mem_.stopped()) {
        // ecall 在 commit_one 中写 IO_TIMER 停机；这里仍把当前周期计入统计。
        ++cycles_;
        mem_.set_cycles(cycles_);
        return false;
    }

    // 4. ROB 头部按序提交后，保留站中操作数齐全的指令可以发射执行。
    // commit 放在 issue 前，可以让刚释放/更新的状态在同一周期影响后续调度。
    start_ready_ops();

    if (!halt_when_empty_ && !fetch_stalled_) {
        // 5. 前端每周期最多分配一条指令到 ROB 和对应保留站。
        if (!allocate_one(error)) return false;
    } else if (trace_enabled_ && fetch_stalled_) {
        trace_stall("控制指令未决，暂停取指");
    }

    ++cycles_;
    mem_.set_cycles(cycles_);

    return !mem_.stopped();
}
bool OoOCpu::run(u64 max_cycles, std::string *error) {
    // max_cycles 是防死循环保护；程序若没有 ecall 或 MMIO 停机，最终会到达上限。
    for (u64 i = 0; i < max_cycles; ++i) {
        if (!step(error)) return mem_.stopped();
    }
    if (error) *error = "cycle limit reached";
    return false;
}
bool OoOCpu::is_empty() const {
    // 判断微结构是否完全排空，主要用于 ecall 等需要等待前序指令提交的场景。
    return alu_rs_.empty() && ls_rs_.empty() && branch_rs_.empty() &&
           rob_.empty() && !alu_fu_[0].busy && !alu_fu_[1].busy && !ls_fu_.busy && !branch_fu_.busy;
}

} // namespace riscv_sim
