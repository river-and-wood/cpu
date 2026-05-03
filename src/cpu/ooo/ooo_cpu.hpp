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
// - rob：Reorder Buffer，执行完成先写 ROB，再按程序顺序提交架构状态。
class OoOCpu {
public:
    explicit OoOCpu(Memory &memory);

    // 清空微结构状态，并从指定 PC 开始重新取指。
    void reset(u32 pc = 0);

    // 打开人类可读 trace；trace_json 会输出 JSON Lines 事件流。
    void set_trace(bool enabled);
    void set_trace_json(bool enabled);

    // 推进一个乱序 CPU 周期：完成、广播、提交、发射、取指/分配。
    // 注意这里是“周期级”模拟，不是一次执行完一条指令；同一周期内可能有
    // 多个执行槽完成，也可能因为 ROB/保留站满而没有新指令进入后端。
    bool step(std::string *error);

    // 连续运行直到 ecall 停机、错误或达到周期上限。
    bool run(u64 max_cycles, std::string *error);
    void dump_registers(std::ostream &out) const;

    // 架构状态和性能计数器观察接口。
    u32 reg(unsigned index) const;
    u32 pc() const;
    u64 cycles() const;
    u64 committed_insts() const;
    u64 branch_predictions() const;
    u64 branch_mispredictions() const;
    u64 flushes() const;
    u64 rs_stalls() const;
    u64 rob_stalls() const;
    u64 store_commits() const;
    double ipc() const;

    std::size_t alu_rs_size() const;
    std::size_t ls_rs_size() const;
    std::size_t branch_rs_size() const;
    std::size_t rob_size() const;
    u64 icache_hits() const;
    u64 icache_misses() const;
    double icache_hit_rate() const;

private:
    // 执行资源类型。每类资源有独立保留站和执行单元。
    // UnitKind 决定微操作进入哪个保留站，也决定 scheduler 从哪个执行槽启动它。
    enum class UnitKind { Alu, LoadStore, Branch };

    // 内部微操作类型，基本与 OpcodeKind 对齐，便于以后扩展。
    // 当前项目没有把一条复杂指令拆成多个 uop，所以一个 DecodedInst 对应一个 MicroOp。
    enum class OpKind {
        Invalid,
        Lui, Auipc, Jal, Jalr,
        Branch, Load, Store, OpImm, Op, System
    };

    // 保留站中的源操作数。
    // ready=false 时 value 无效，tag 指向生产该值的 ROB 项。
    // 这就是 Tomasulo/CDB 风格的数据相关表示：消费者不直接等寄存器名，
    // 而是等待生产者广播同一个 tag。
    struct Operand {
        bool ready = true; // 操作数是否已经可用
        u32 value = 0;     // 操作数值
        u32 tag = 0;       // 等待的结果标签，0 表示不等待
    };

    // 架构寄存器和重命名标签。
    // tag!=0 表示该寄存器最新值尚未按序提交，需要等待对应 ROB。
    // value 是已经提交的架构值；tag 是正在路上的最新写者。
    // 如果某个寄存器连续被多条指令写，tag 会指向最新那条指令，解决 WAW/WAR。
    struct RegSlot {
        u32 value = 0; // 架构寄存器值
        u32 tag = 0;   // 最新写入者标签，0 表示未锁定
    };

    // 已解码并分配到后端的微操作。
    // src1/src2 记录 RAW 依赖，dst_tag 是该指令在 ROB/CDB 中的唯一编号。
    // predicted_pc 记录前端当时选择的下一 PC，执行阶段算出 actual PC 后用于判断是否 flush。
    struct MicroOp {
        bool valid = false;              // 当前槽位是否有效
        DecodedInst inst;                // 原始解码结果
        u32 pc = 0;                      // 该指令的取指 PC
        u32 predicted_pc = 0;            // 分支/跳转预测后的下一条 PC
        UnitKind unit = UnitKind::Alu;   // 目标执行单元类型
        OpKind op = OpKind::Invalid;     // 微操作类别
        Operand src1;                    // 第一个源操作数
        Operand src2;                    // 第二个源操作数
        u8 rd = 0;                       // 目的寄存器编号
        u32 dst_tag = 0; // ROB/tag 编号，用于写回匹配和 RAW 唤醒
    };

    // 执行单元完成后广播到 ROB 和保留站的结果。
    // ALU/Load/JAL/JALR 会设置 has_value；Store 会设置 is_store。
    // Branch 通常只可能产生 Redirect，不需要写寄存器值。
    struct WriteBack {
        bool valid = false;      // 是否有有效广播
        u32 tag = 0;             // 对应生产者 ROB tag
        u8 rd = 0;               // 目的寄存器编号，仅 has_value 时有意义
        u32 value = 0;           // ALU/Load/跳转链接结果
        bool has_value = false;  // 是否产生寄存器写回值
        bool is_store = false;   // 是否为 Store 地址/数据结果
        u32 store_addr = 0;      // Store 目标地址
        u32 store_value = 0;     // Store 待提交数据
    };

    // 控制流预测错误时的重定向请求。
    // tag 用来区分“谁导致了重定向”，flush 时只清除它之后的年轻指令。
    struct Redirect {
        bool valid = false; // 是否需要 flush
        u32 tag = 0;        // 发生重定向的指令 tag；只清除更年轻指令
        u32 pc = 0;         // 正确下一条 PC
    };

    // ROB 项保存“已完成但未提交”的结果，用于保证架构状态按程序顺序更新。
    // 执行可以乱序完成，但寄存器文件和内存必须按 ROB 队头顺序更新。
    struct RobEntry {
        bool valid = false;      // ROB 项是否有效
        bool ready = false;      // 执行结果是否已经写回
        bool has_value = false;  // 是否需要提交到寄存器
        bool is_store = false;   // 是否为 Store，内存写入必须等到提交
        bool is_ecall = false;   // ecall 停机指令
        bool is_control = false; // 分支/JAL/JALR 类控制指令
        u32 tag = 0;             // 全局唯一 ROB tag
        u8 rd = 0;               // 目的寄存器
        u32 value = 0;           // 待提交寄存器值
        u32 pc = 0;              // 指令 PC
        u32 predicted_pc = 0;    // 预测下一条 PC
        u32 actual_pc = 0;       // 实际下一条 PC
        DecodedInst inst;
        u32 store_addr = 0;      // Store 提交地址
        u32 store_value = 0;     // Store 提交数据
    };

    // 性能统计计数器，供命令行和测试读取。
    // 这些计数器不参与功能正确性，只用于观察乱序模型的行为。
    struct Stats {
        u64 allocated = 0;              // 已分配到后端的指令数
        u64 committed = 0;              // 已按序提交指令数
        u64 branch_predictions = 0;     // 做过预测的控制指令数
        u64 branch_mispredictions = 0;  // 预测错误次数
        u64 flushes = 0;                // flush 次数
        u64 rs_stalls = 0;              // 保留站满导致的停顿
        u64 rob_stalls = 0;             // ROB 满导致的停顿
        u64 store_commits = 0;          // 真正写入内存的 Store 数
    };

    // 正在执行单元里运行的微操作槽位。
    // remain 表示还要经过多少次 complete_exec_slots 递减才算完成。
    struct ExecSlot {
        bool busy = false;
        unsigned remain = 0; // 剩余执行周期
        MicroOp op;
    };

    // 极简两路组相联指令缓存。
    // 只缓存 32 位指令字，用 PC[8:2] 作为 index，记录命中/缺失统计。
    // 这个 ICache 只影响统计，不额外增加 miss penalty，所以不会改变执行结果。
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
    InstructionCache icache_; // 取指路径上的指令缓存
    RegSlot regs_[32];        // 带重命名标签的架构寄存器文件
    u32 pc_;                  // 前端下一次取指 PC
    u64 cycles_;              // 已模拟周期数
    u32 next_tag_;            // 下一个 ROB/tag 编号
    bool fetch_stalled_;     // JALR/ecall 等需要停止继续取指的边界
    bool halt_when_empty_;   // ecall 已进入 ROB，等待按序提交
    bool trace_enabled_;     // 是否输出可视化执行日志
    bool trace_json_;        // 是否输出 JSON Lines trace

    std::vector<MicroOp> alu_rs_;    // ALU 保留站
    std::vector<MicroOp> ls_rs_;     // Load/Store 保留站
    std::vector<MicroOp> branch_rs_; // Branch 保留站
    std::vector<RobEntry> rob_;      // 重排序缓冲区
    ExecSlot alu_fu_[2];             // 两个 ALU 执行单元
    ExecSlot ls_fu_;                 // 一个 Load/Store 执行单元
    ExecSlot branch_fu_;             // 一个分支执行单元
    Stats stats_;

    // 内部流水线阶段和辅助函数。
    void clear();
    bool is_empty() const;
    bool allocate_one(std::string *error);
    bool decode_to_micro_op(const DecodedInst &inst, u32 pc, MicroOp *out, std::string *error);
    Operand read_operand(u8 reg) const;
    const RobEntry *find_rob(u32 tag) const;
    RobEntry *find_rob(u32 tag);
    bool enqueue_rs(const MicroOp &op);
    bool rs_full(UnitKind unit) const;
    void start_ready_ops();
    bool complete_exec_slots(std::vector<WriteBack> *wbs, Redirect *redirect, std::string *error);
    void broadcast(const std::vector<WriteBack> &wbs);
    void apply_writeback_to_rob(const WriteBack &wb);
    bool commit_one(std::string *error);
    void flush_younger_than(u32 tag);
    void rebuild_rename_state();
    bool has_older_store(u32 tag) const;
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
    void trace_commit(const RobEntry &entry) const;
    void trace_flush(u32 tag, u32 pc) const;
    void trace_redirect(u32 pc) const;
};

} // namespace riscv_sim

#endif
