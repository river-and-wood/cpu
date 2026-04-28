#ifndef CPP_SIM_CPU_HPP
#define CPP_SIM_CPU_HPP

#include "common.hpp"
#include "memory.hpp"

#include <iosfwd>
#include <string>

namespace riscv_sim {

class Cpu {
public:
    explicit Cpu(Memory &memory);

    void reset(u32 pc = 0);
    bool step(std::string *error);
    bool run(u64 max_cycles, std::string *error);
    void dump_registers(std::ostream &out) const;
    u32 reg(unsigned index) const;
    u32 pc() const;
    u64 cycles() const;

private:
    Memory &mem_;
    u32 regs_[32];
    u32 pc_;
    u64 cycles_;

    void write_reg(u8 rd, u32 value);
};

} // namespace riscv_sim

#endif
