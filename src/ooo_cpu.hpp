#ifndef CPP_SIM_OOO_CPU_HPP
#define CPP_SIM_OOO_CPU_HPP

#include "common.hpp"
#include "memory.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace riscv_sim {

// 乱序 CPU 的功能级微结构模型。
//
// 变量名释义：
// - tag：结果标签，相当于 Verilog 中 RegStat/RS 使用的寄存器锁。
// - rs：Reservation Station，保留站，等待操作数就绪的指令队列。
// - wb：Write Back，写回广播，一条执行完成的结果。
// - fu：Functional Unit，执行单元，例如 ALU/LS/Branch。
// - rob 这里没有实现：原 Verilog 项目没有完整 ROB，本模型通过控制指令暂停取指避免错误路径提交。
class OoOCpu {
public:
    explicit OoOCpu(Memory &memory);

    void reset(u32 pc = 0);
    void set_trace(bool enabled);
    bool step(std::string *error);
    bool run(u64 max_cycles, std::string *error);
    void dump_registers(std::ostream &out) const;

    u32 reg(unsigned index) const;
    u32 pc() const;
    u64 cycles() const;

    std::size_t alu_rs_size() const;
    std::size_t ls_rs_size() const;
    std::size_t branch_rs_size() const;
    u64 icache_hits() const;
    u64 icache_misses() const;

private:
    enum class UnitKind { Alu, LoadStore, Branch };
    enum class OpKind {
        Invalid,
        Lui, Auipc, Jal, Jalr,
        Branch, Load, Store, OpImm, Op, System
    };

    struct Operand {
        bool ready = true; // 操作数是否已经可用
        u32 value = 0;     // 操作数值
        u32 tag = 0;       // 等待的结果标签，0 表示不等待
    };

    struct RegSlot {
        u32 value = 0; // 架构寄存器值
        u32 tag = 0;   // 最新写入者标签，0 表示未锁定
    };

    struct MicroOp {
        bool valid = false;
        DecodedInst inst;
        u32 pc = 0;
        UnitKind unit = UnitKind::Alu;
        OpKind op = OpKind::Invalid;
        Operand src1;
        Operand src2;
        u8 rd = 0;
        u32 dst_tag = 0; // 目标寄存器标签，用于写回匹配
    };

    struct WriteBack {
        bool valid = false;
        u32 tag = 0;
        u8 rd = 0;
        u32 value = 0;
    };

    struct Redirect {
        bool valid = false;
        u32 pc = 0;
    };

    struct ExecSlot {
        bool busy = false;
        unsigned remain = 0; // 剩余执行周期
        MicroOp op;
    };

    class InstructionCache {
    public:
        InstructionCache();
        void reset();
        u32 fetch(Memory &memory, u32 pc);
        u64 hits() const;
        u64 misses() const;
    private:
        struct Line {
            bool valid = false;
            u32 tag = 0;
            u32 data = 0;
        };
        std::vector<Line> way0_;
        std::vector<Line> way1_;
        std::vector<bool> replace_way_;
        u64 hits_;
        u64 misses_;
    };

    Memory &mem_;
    InstructionCache icache_;
    RegSlot regs_[32];
    u32 pc_;
    u64 cycles_;
    u32 next_tag_;
    bool fetch_stalled_;     // 控制指令在飞行时暂停取指
    bool halt_when_empty_;   // ecall 已到达，等待前序乱序操作排空
    bool trace_enabled_;     // 是否输出可视化执行日志

    std::vector<MicroOp> alu_rs_;
    std::vector<MicroOp> ls_rs_;
    std::vector<MicroOp> branch_rs_;
    ExecSlot alu_fu_[2];
    ExecSlot ls_fu_;
    ExecSlot branch_fu_;

    void clear();
    bool is_empty() const;
    bool allocate_one(std::string *error);
    bool decode_to_micro_op(const DecodedInst &inst, u32 pc, MicroOp *out, std::string *error);
    Operand read_operand(u8 reg) const;
    bool enqueue_rs(const MicroOp &op);
    void start_ready_ops();
    void complete_exec_slots(std::vector<WriteBack> *wbs, Redirect *redirect);
    void broadcast(const std::vector<WriteBack> &wbs);
    void apply_writeback(const WriteBack &wb);
    void update_waiting_operand(Operand *operand, const WriteBack &wb);
    void erase_started(std::vector<MicroOp> *rs, std::size_t index);

    bool execute_result(const MicroOp &op, WriteBack *wb, Redirect *redirect, std::string *error);
    unsigned latency(const MicroOp &op) const;
    bool src_ready(const MicroOp &op) const;
    const char *unit_name(UnitKind unit) const;
    void trace_cycle_begin() const;
    void trace_allocate(const MicroOp &op) const;
    void trace_stall(const char *reason) const;
    void trace_start(const ExecSlot &slot, unsigned fu_index) const;
    void trace_writeback(const WriteBack &wb) const;
    void trace_redirect(u32 pc) const;
};

} // namespace riscv_sim

#endif
