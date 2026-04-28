#include "memory.hpp"

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace riscv_sim {

Memory::Memory() : ram_(RAM_SIZE, 0), stopped_(false), cycles_(0) {}

void Memory::reset() {
    std::fill(ram_.begin(), ram_.end(), 0);
    stopped_ = false;
    cycles_ = 0;
}

bool Memory::load_hex_file(const std::string &path, u32 base_addr, std::string *error) {
    std::ifstream in(path.c_str());
    if (!in) {
        if (error) *error = "cannot open hex file: " + path;
        return false;
    }

    std::string token;
    u32 addr = base_addr;
    while (in >> token) {
        if (token.empty() || token[0] == '#') {
            std::string ignored;
            std::getline(in, ignored);
            continue;
        }
        char *end = 0;
        errno = 0;
        unsigned long value = std::strtoul(token.c_str(), &end, 16);
        if (errno || end == token.c_str() || *end != '\0') {
            if (error) *error = "bad hex token: " + token;
            return false;
        }
        if (token.size() <= 2) {
            write8(addr, static_cast<u8>(value));
            addr += 1;
        } else {
            write32(addr, static_cast<u32>(value));
            addr += 4;
        }
    }
    return true;
}

void Memory::load_words(const std::vector<u32> &words, u32 base_addr) {
    for (std::size_t i = 0; i < words.size(); ++i) {
        write32(base_addr + static_cast<u32>(i * 4u), words[i]);
    }
}

u8 Memory::read8(u32 addr) const {
    if (is_ram(addr)) return ram_[addr];
    if (addr >= IO_TIMER && addr < IO_TIMER + 4u) {
        return static_cast<u8>((cycles_ >> ((addr - IO_TIMER) * 8u)) & 0xffu);
    }
    return 0;
}

u16 Memory::read16(u32 addr) const {
    return static_cast<u16>(read8(addr) | (static_cast<u16>(read8(addr + 1u)) << 8));
}

u32 Memory::read32(u32 addr) const {
    return static_cast<u32>(read8(addr)) |
           (static_cast<u32>(read8(addr + 1u)) << 8) |
           (static_cast<u32>(read8(addr + 2u)) << 16) |
           (static_cast<u32>(read8(addr + 3u)) << 24);
}

void Memory::write8(u32 addr, u8 value) {
    if (is_ram(addr)) {
        ram_[addr] = value;
        return;
    }
    if (addr == IO_TIMER) {
        stopped_ = true;
    }
}

void Memory::write16(u32 addr, u16 value) {
    write8(addr, static_cast<u8>(value & 0xffu));
    write8(addr + 1u, static_cast<u8>((value >> 8) & 0xffu));
}

void Memory::write32(u32 addr, u32 value) {
    write8(addr, static_cast<u8>(value & 0xffu));
    write8(addr + 1u, static_cast<u8>((value >> 8) & 0xffu));
    write8(addr + 2u, static_cast<u8>((value >> 16) & 0xffu));
    write8(addr + 3u, static_cast<u8>((value >> 24) & 0xffu));
}

bool Memory::stopped() const { return stopped_; }
u64 Memory::cycles() const { return cycles_; }
void Memory::set_cycles(u64 value) { cycles_ = value; }

bool Memory::is_ram(u32 addr) const {
    return addr < ram_.size();
}

} // namespace riscv_sim
