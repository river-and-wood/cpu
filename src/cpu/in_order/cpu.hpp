#ifndef CPP_SIM_CPU_HPP
#define CPP_SIM_CPU_HPP

#include "common.hpp"
#include "memory.hpp"

#include <iosfwd>
#include <string>

namespace riscv_sim {

// 顺序执行的 RV32 模拟器。
// 它按“取指 -> 解码 -> 执行 -> 写回 -> 更新 PC”的单周期方式解释指令，
// 主要用于功能正确性参考和与乱序模型对比。
class Cpu {
public:
    explicit Cpu(Memory &memory);

    // 从指定 PC 重新开始执行，并清空寄存器/周期计数。
    void reset(u32 pc = 0);

    // 执行一条指令；发生不支持指令时返回 false 并写入 error。
    bool step(std::string *error);

    // 最多执行 max_cycles 条指令/周期，程序正常停机返回 true。
    bool run(u64 max_cycles, std::string *error);

    // 按 x0..x31 输出架构寄存器。
    void dump_registers(std::ostream &out) const;

    // 只读观察接口，用于测试和命令行输出。
    u32 reg(unsigned index) const;
    u32 pc() const;
    u64 cycles() const;

private:
    Memory &mem_;  // CPU 访问的内存/MMIO
    u32 regs_[32]; // 32 个架构寄存器，x0 始终保持 0
    u32 pc_;       // 当前取指地址
    u64 cycles_;   // 已执行周期数

    // 写架构寄存器；自动忽略对 x0 的写入。
    void write_reg(u8 rd, u32 value);
};

} // namespace riscv_sim

#endif
