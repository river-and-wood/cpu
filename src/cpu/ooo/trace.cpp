#include "ooo_cpu.hpp"
#include <iostream>

namespace riscv_sim {

// trace.cpp 集中输出乱序流水事件，支持人类可读文本和前端可消费的 JSON Lines。
// 这些函数只观察状态，不改变 CPU 模拟结果。
const char *OoOCpu::unit_name(UnitKind unit) const {
    // trace 输出使用短名字，减少每周期日志长度。
    switch (unit) {
    case UnitKind::Alu: return "ALU";
    case UnitKind::LoadStore: return "LS";
    case UnitKind::Branch: return "BR";
    }
    return "?";
}
void OoOCpu::trace_cycle_begin() const {
    if (trace_json_) {
        // JSON Lines 模式每行一个独立事件，方便 Web 前端或脚本逐行解析。
        std::cout << "{\"event\":\"cycle\",\"cycle\":" << cycles_
                  << ",\"pc\":" << pc_
                  << ",\"rob\":" << rob_.size()
                  << ",\"alu_rs\":" << alu_rs_.size()
                  << ",\"ls_rs\":" << ls_rs_.size()
                  << ",\"branch_rs\":" << branch_rs_.size() << "}\n";
        return;
    }
    std::cout << "[cycle " << cycles_ << "] pc=0x" << std::hex << pc_ << std::dec
              << " | ROB=" << rob_.size()
              << " | ALU_RS=" << alu_rs_.size()
              << " LS_RS=" << ls_rs_.size()
              << " BR_RS=" << branch_rs_.size()
              << " | FU{alu0=" << (alu_fu_[0].busy ? "busy" : "idle")
              << ", alu1=" << (alu_fu_[1].busy ? "busy" : "idle")
              << ", ls=" << (ls_fu_.busy ? "busy" : "idle")
              << ", br=" << (branch_fu_.busy ? "busy" : "idle")
              << "}\n";
}
void OoOCpu::trace_allocate(const MicroOp &op) const {
    if (trace_json_) {
        // allocate 事件记录源操作数是否 ready，用来观察 RAW 依赖是否被 tag 捕获。
        std::cout << "{\"event\":\"allocate\",\"cycle\":" << cycles_
                  << ",\"pc\":" << op.pc
                  << ",\"inst\":" << op.inst.raw
                  << ",\"tag\":" << op.dst_tag
                  << ",\"rd\":" << unsigned(op.rd)
                  << ",\"rs1\":" << unsigned(op.inst.rs1)
                  << ",\"rs2\":" << unsigned(op.inst.rs2)
                  << ",\"src1_ready\":" << (op.src1.ready ? "true" : "false")
                  << ",\"src1_tag\":" << op.src1.tag
                  << ",\"src2_ready\":" << (op.src2.ready ? "true" : "false")
                  << ",\"src2_tag\":" << op.src2.tag
                  << ",\"unit\":\"" << unit_name(op.unit) << "\""
                  << ",\"predicted_pc\":" << op.predicted_pc << "}\n";
        return;
    }
    std::cout << "  分配: pc=0x" << std::hex << op.pc
              << " inst=0x" << op.inst.raw << std::dec
              << " -> " << unit_name(op.unit);
    if (op.dst_tag != 0)
        std::cout << " rd=x" << unsigned(op.rd) << " tag=T" << op.dst_tag;
    if (!op.src1.ready)
        std::cout << " src1等待T" << op.src1.tag;
    if (!op.src2.ready)
        std::cout << " src2等待T" << op.src2.tag;
    std::cout << "\n";
}
void OoOCpu::trace_stall(const char *reason) const {
    if (trace_json_) {
        std::cout << "{\"event\":\"stall\",\"cycle\":" << cycles_
                  << ",\"reason\":\"" << reason << "\"}\n";
        return;
    }
    std::cout << "  停顿: " << reason << "\n";
}
void OoOCpu::trace_start(const ExecSlot &slot, unsigned fu_index) const {
    // fu_index 与 scheduler.cpp 中 trace_start 的调用约定一致：0/1/2/3 对应 ALU0/ALU1/LS/BR。
    const char *fu_name = (fu_index == 0) ? "ALU0" :
                          (fu_index == 1) ? "ALU1" :
                          (fu_index == 2) ? "LS" : "BR";
    if (trace_json_) {
        std::cout << "{\"event\":\"issue\",\"cycle\":" << cycles_
                  << ",\"fu\":\"" << fu_name << "\""
                  << ",\"pc\":" << slot.op.pc
                  << ",\"inst\":" << slot.op.inst.raw
                  << ",\"tag\":" << slot.op.dst_tag
                  << ",\"latency\":" << slot.remain << "}\n";
        return;
    }
    std::cout << "  发射执行: " << fu_name
              << " pc=0x" << std::hex << slot.op.pc
              << " inst=0x" << slot.op.inst.raw << std::dec
              << " latency=" << slot.remain << "\n";
}
void OoOCpu::trace_writeback(const WriteBack &wb) const {
    if (trace_json_) {
        // WriteBack 可能是寄存器结果，也可能是 Store 地址/值，字段按类型选择输出。
        std::cout << "{\"event\":\"writeback\",\"cycle\":" << cycles_
                  << ",\"tag\":" << wb.tag;
        if (wb.has_value) {
            std::cout << ",\"rd\":" << unsigned(wb.rd)
                      << ",\"value\":" << wb.value;
        }
        if (wb.is_store) {
            std::cout << ",\"store_addr\":" << wb.store_addr
                      << ",\"store_value\":" << wb.store_value;
        }
        std::cout << "}\n";
        return;
    }
    std::cout << "  写回广播: T" << wb.tag
              << " ready";
    if (wb.has_value)
        std::cout << " -> x" << unsigned(wb.rd)
                  << " = 0x" << std::hex << wb.value << std::dec;
    if (wb.is_store)
        std::cout << " store[0x" << std::hex << wb.store_addr << "]" << std::dec;
    std::cout << "\n";
}
void OoOCpu::trace_commit(const RobEntry &entry) const {
    if (trace_json_) {
        // commit 事件表示架构状态真正改变；它和 writeback 可能相隔多个周期。
        std::cout << "{\"event\":\"commit\",\"cycle\":" << cycles_
                  << ",\"tag\":" << entry.tag
                  << ",\"pc\":" << entry.pc;
        if (entry.has_value) {
            std::cout << ",\"rd\":" << unsigned(entry.rd)
                      << ",\"value\":" << entry.value;
        }
        if (entry.is_store) {
            std::cout << ",\"store_addr\":" << entry.store_addr
                      << ",\"store_value\":" << entry.store_value;
        }
        if (entry.is_ecall) std::cout << ",\"ecall\":true";
        std::cout << "}\n";
        return;
    }
    std::cout << "  提交: T" << entry.tag << " pc=0x" << std::hex << entry.pc << std::dec;
    if (entry.has_value)
        std::cout << " -> x" << unsigned(entry.rd) << "=0x" << std::hex << entry.value << std::dec;
    if (entry.is_store)
        std::cout << " store[0x" << std::hex << entry.store_addr << "]" << std::dec;
    if (entry.is_ecall) std::cout << " ecall";
    std::cout << "\n";
}
void OoOCpu::trace_flush(u32 tag, u32 pc) const {
    if (trace_json_) {
        std::cout << "{\"event\":\"flush\",\"cycle\":" << cycles_
                  << ",\"after_tag\":" << tag
                  << ",\"pc\":" << pc << "}\n";
        return;
    }
    std::cout << "  Flush: 清除 T" << tag << " 之后的推测指令，pc=0x"
              << std::hex << pc << std::dec << "\n";
}
void OoOCpu::trace_redirect(u32 pc) const {
    if (trace_json_) {
        std::cout << "{\"event\":\"redirect\",\"cycle\":" << cycles_
                  << ",\"pc\":" << pc << "}\n";
        return;
    }
    std::cout << "  控制重定向: next_pc=0x" << std::hex << pc << std::dec << "\n";
}

} // namespace riscv_sim
