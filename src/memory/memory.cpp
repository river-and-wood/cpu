#include "memory.hpp"

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace riscv_sim {

Memory::Memory() : ram_(RAM_SIZE, 0), stopped_(false), cycles_(0) {}

void Memory::reset() {
    // std::fill 把整块 RAM 清零；vector 大小不变，只重置内容。
    std::fill(ram_.begin(), ram_.end(), 0);
    stopped_ = false;
    cycles_ = 0;
}

bool Memory::load_hex_file(const std::string &path, u32 base_addr, std::string *error) {
    // ifstream 负责打开文本文件；后面用 operator>> 按空白分隔读取 token。
    std::ifstream in(path.c_str());
    if (!in) {
        if (error) *error = "cannot open hex file: " + path;
        return false;
    }

    std::string token;
    u32 addr = base_addr;
    while (in >> token) {
        // 支持以 # 开头的注释行，便于手写测试程序。
        if (token.empty() || token[0] == '#') {
            std::string ignored;
            std::getline(in, ignored);
            continue;
        }
        char *end = 0;
        errno = 0;
        // strtoul 的 base=16 表示按十六进制解析；end 用来检查 token 是否完全合法。
        unsigned long value = std::strtoul(token.c_str(), &end, 16);
        if (errno || end == token.c_str() || *end != '\0') {
            if (error) *error = "bad hex token: " + token;
            return false;
        }
        // 两位及以下 token 按字节加载；更长 token 按 32 位小端指令字加载。
        // 这样既能写 13 00 a0 00 这种字节流，也能写 00a00093 这种指令字。
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
    // 内置测试程序已经是 32 位指令字，按 4 字节递增写入内存。
    for (std::size_t i = 0; i < words.size(); ++i) {
        write32(base_addr + static_cast<u32>(i * 4u), words[i]);
    }
}

u8 Memory::read8(u32 addr) const {
    // 普通 RAM 地址直接索引 vector；u32 地址值在 RAM_SIZE 范围内才有效。
    if (is_ram(addr)) return ram_[addr];
    if (addr >= IO_TIMER && addr < IO_TIMER + 4u) {
        // IO_TIMER 的四个字节映射到 cycles_ 低 32 位，模拟计时器 MMIO。
        return static_cast<u8>((cycles_ >> ((addr - IO_TIMER) * 8u)) & 0xffu);
    }
    return 0;
}

u16 Memory::read16(u32 addr) const {
    // RISC-V 默认小端：低地址保存低 8 位，高地址保存高 8 位。
    return static_cast<u16>(read8(addr) | (static_cast<u16>(read8(addr + 1u)) << 8));
}

u32 Memory::read32(u32 addr) const {
    // 32 位读取由 4 次 read8 组合而成，因此 MMIO 地址也能复用同一逻辑。
    return static_cast<u32>(read8(addr)) |
           (static_cast<u32>(read8(addr + 1u)) << 8) |
           (static_cast<u32>(read8(addr + 2u)) << 16) |
           (static_cast<u32>(read8(addr + 3u)) << 24);
}

void Memory::write8(u32 addr, u8 value) {
    // 超出 RAM 且不是 IO_TIMER 的地址会被静默忽略，简化异常处理。
    if (is_ram(addr)) {
        ram_[addr] = value;
        return;
    }
    if (addr == IO_TIMER) {
        // 向 IO_TIMER 写任意值都表示程序请求停机。
        stopped_ = true;
    }
}

void Memory::write16(u32 addr, u16 value) {
    // 写入同样按小端拆成字节，避免依赖宿主机器的内存端序。
    write8(addr, static_cast<u8>(value & 0xffu));
    write8(addr + 1u, static_cast<u8>((value >> 8) & 0xffu));
}

void Memory::write32(u32 addr, u32 value) {
    // Store word 和程序加载都会走这里；若 addr 是 IO_TIMER，会先写低字节并触发停机。
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
