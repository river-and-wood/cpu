#include "ooo_cpu.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using riscv_sim::Memory;
using riscv_sim::OoOCpu;
using riscv_sim::u32;
using riscv_sim::u64;

namespace {

std::vector<u32> default_program() {
    std::vector<u32> p;
    p.push_back(0x00a00093u); // addi x1,x0,10
    p.push_back(0x01400113u); // addi x2,x0,20
    p.push_back(0x002081b3u); // add  x3,x1,x2
    p.push_back(0x00000073u); // ecall
    return p;
}

void usage(const char *argv0) {
    std::cerr << "usage: " << argv0 << " [program.hex] [--max-cycles N] [--pc N] [--trace]\n";
}

bool parse_u64(const std::string &text, u64 *out) {
    char *end = 0;
    unsigned long long value = std::strtoull(text.c_str(), &end, 0);
    if (end == text.c_str() || *end != '\0') return false;
    *out = static_cast<u64>(value);
    return true;
}

} // namespace

int main(int argc, char **argv) {
    std::string hex_path;
    u64 max_cycles = 100000;
    u64 start_pc64 = 0;
    bool trace = false;

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
        } else if (arg == "--trace") {
            trace = true;
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
        memory.load_words(default_program(), 0);
    } else if (!memory.load_hex_file(hex_path, 0, &error)) {
        std::cerr << error << "\n";
        return 1;
    }

    OoOCpu cpu(memory);
    cpu.reset(static_cast<u32>(start_pc64));
    cpu.set_trace(trace);

    const bool ok = cpu.run(max_cycles, &error);
    std::cout << "cycles: " << cpu.cycles() << "\n";
    std::cout << "pc: 0x" << std::hex << cpu.pc() << std::dec << "\n";
    std::cout << "icache_hits: " << cpu.icache_hits() << "\n";
    std::cout << "icache_misses: " << cpu.icache_misses() << "\n";
    cpu.dump_registers(std::cout);

    if (!ok) {
        std::cerr << error << "\n";
        return 1;
    }
    return 0;
}
