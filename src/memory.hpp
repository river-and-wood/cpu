#ifndef CPP_SIM_MEMORY_HPP
#define CPP_SIM_MEMORY_HPP

#include "common.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace riscv_sim {

class Memory {
public:
    Memory();

    void reset();
    bool load_hex_file(const std::string &path, u32 base_addr, std::string *error);
    void load_words(const std::vector<u32> &words, u32 base_addr);

    u8 read8(u32 addr) const;
    u16 read16(u32 addr) const;
    u32 read32(u32 addr) const;
    void write8(u32 addr, u8 value);
    void write16(u32 addr, u16 value);
    void write32(u32 addr, u32 value);

    bool stopped() const;
    u64 cycles() const;
    void set_cycles(u64 value);

private:
    std::vector<u8> ram_;
    bool stopped_;
    u64 cycles_;

    bool is_ram(u32 addr) const;
};

} // namespace riscv_sim

#endif
