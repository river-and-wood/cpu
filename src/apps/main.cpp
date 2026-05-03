#include "cpu.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using riscv_sim::Cpu;
using riscv_sim::Memory;
using riscv_sim::u32;
using riscv_sim::u64;

namespace {

// 没有指定 hex 文件时运行的最小示例程序，用来验证顺序 CPU 基本功能。
std::vector<u32> default_program() {
    std::vector<u32> p;
    // 这些常量是已经编码好的 RV32 指令，不是 C++ 表达式。
    // 运行结果应把 10 + 20 写到 x3，并通过 ecall 停止模拟器。
    p.push_back(0x00a00093u); // addi x1,x0,10
    p.push_back(0x01400113u); // addi x2,x0,20
    p.push_back(0x002081b3u); // add  x3,x1,x2
    p.push_back(0x00302023u); // sw   x3,0(x0)
    p.push_back(0x00002183u); // lw   x3,0(x0)
    p.push_back(0x00030037u); // lui  x0,0x30 (nop because rd=x0)
    p.push_back(0x00000073u); // ecall, simulator stop
    return p;
}

void usage(const char *argv0) {
    std::cerr << "usage: " << argv0 << " [program.hex] [--max-cycles N] [--pc N]\n"
              << "hex format: one 32-bit instruction word per token, or byte tokens.\n";
}

// 解析十进制或 0x 前缀整数，供命令行周期数/起始 PC 使用。
bool parse_u64(const std::string &text, u64 *out) {
    char *end = 0;
    // base=0 让 C 库自动识别十进制、0x 十六进制等常见写法。
    unsigned long long value = std::strtoull(text.c_str(), &end, 0);
    if (end == text.c_str() || *end != '\0') return false;
    *out = static_cast<u64>(value);
    return true;
}

} // namespace

int main(int argc, char **argv) {
    // hex_path 为空表示使用内置默认程序；非空时从用户传入的 hex 文件加载。
    std::string hex_path;
    u64 max_cycles = 100000;
    u64 start_pc64 = 0;

    // 命令行参数保持简单：可选程序路径、最大周期数和起始 PC。
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            usage(argv[0]);
            return 0;
        } else if (arg == "--max-cycles" && i + 1 < argc) {
            if (!parse_u64(argv[++i], &max_cycles)) {
                usage(argv[0]);
                return 2;
            }
        } else if (arg == "--pc" && i + 1 < argc) {
            if (!parse_u64(argv[++i], &start_pc64)) {
                usage(argv[0]);
                return 2;
            }
        } else if (hex_path.empty()) {
            hex_path = arg;
        } else {
            usage(argv[0]);
            return 2;
        }
    }

    Memory memory;
    std::string error;
    if (hex_path.empty()) {
        // 未传入文件时，直接把内置指令写入地址 0。
        memory.load_words(default_program(), 0);
    } else if (!memory.load_hex_file(hex_path, 0, &error)) {
        // load_hex_file 失败时会把具体原因写到 error，main 只负责打印和返回非 0。
        std::cerr << error << "\n";
        return 1;
    }

    Cpu cpu(memory);
    // reset 会清空 CPU 寄存器/周期计数；Memory 内容不会被清掉，所以程序仍在 RAM 中。
    cpu.reset(static_cast<u32>(start_pc64));

    // 顺序 CPU 的 cycles 基本等于成功执行的指令数。
    const bool ok = cpu.run(max_cycles, &error);
    std::cout << "cycles: " << cpu.cycles() << "\n";
    std::cout << "pc: 0x" << std::hex << cpu.pc() << std::dec << "\n";
    cpu.dump_registers(std::cout);

    if (!ok) {
        std::cerr << error << "\n";
        return 1;
    }
    return 0;
}
