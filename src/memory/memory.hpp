#ifndef CPP_SIM_MEMORY_HPP
#define CPP_SIM_MEMORY_HPP

#include "common.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace riscv_sim {

// 简单字节寻址内存模型。
// 低 RAM_SIZE 字节是普通 RAM，IO_TIMER 地址被映射成停止信号和周期计数读取。
class Memory {
public:
    Memory();

    // 清空 RAM，并把停止标志和周期计数复位。
    void reset();

    // 从文本 hex 文件加载程序；base_addr 是写入内存的起始地址。
    bool load_hex_file(const std::string &path, u32 base_addr, std::string *error);

    // 直接按 32 位小端字写入一组指令/数据，常用于内置测试程序。
    void load_words(const std::vector<u32> &words, u32 base_addr);

    // 小端序读写接口；非法或未映射地址读出 0。
    u8 read8(u32 addr) const;
    u16 read16(u32 addr) const;
    u32 read32(u32 addr) const;
    void write8(u32 addr, u8 value);
    void write16(u32 addr, u16 value);
    void write32(u32 addr, u32 value);

    // stopped 表示程序通过 ecall/MMIO 请求模拟器停止。
    bool stopped() const;

    // CPU 每周期回写该值，IO_TIMER 读取时返回它的低 32 位字节。
    u64 cycles() const;
    void set_cycles(u64 value);

private:
    std::vector<u8> ram_; // 普通 RAM 存储体
    bool stopped_;        // 停机标志
    u64 cycles_;          // 当前 CPU 周期计数

    // 判断地址是否落在普通 RAM 范围内。
    bool is_ram(u32 addr) const;
};

} // namespace riscv_sim

#endif
