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

// 没有指定 hex 文件时运行的最小乱序示例程序。
std::vector<u32> default_program() {
    std::vector<u32> p;
    // 默认程序故意保持很短，便于观察乱序 trace 中的分配、发射、写回、提交。
    p.push_back(0x00a00093u); // addi x1,x0,10
    p.push_back(0x01400113u); // addi x2,x0,20
    p.push_back(0x002081b3u); // add  x3,x1,x2
    p.push_back(0x00000073u); // ecall
    return p;
}

void usage(const char *argv0) {
    std::cerr << "usage: " << argv0
              << " [program.hex] [--max-cycles N] [--pc N] [--trace] [--trace-json] [--stats-json]\n";
}

// 解析命令行数值参数，支持 0x 前缀。
bool parse_u64(const std::string &text, u64 *out) {
    char *end = 0;
    // strtoull 返回 unsigned long long，再收窄到项目统一使用的 u64。
    unsigned long long value = std::strtoull(text.c_str(), &end, 0);
    if (end == text.c_str() || *end != '\0') return false;
    *out = static_cast<u64>(value);
    return true;
}

} // namespace

int main(int argc, char **argv) {
    // 乱序入口比顺序入口多了 trace 和 stats_json 选项，用于观察微结构行为。
    std::string hex_path;
    u64 max_cycles = 100000;
    u64 start_pc64 = 0;
    bool trace = false;
    bool trace_json = false;
    bool stats_json = false;

    // trace 输出流水线事件，stats_json 输出机器可读性能统计。
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
        } else if (arg == "--trace-json") {
            trace_json = true;
        } else if (arg == "--stats-json") {
            stats_json = true;
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
        // 默认程序直接写入内存地址 0，便于无参数演示。
        memory.load_words(default_program(), 0);
    } else if (!memory.load_hex_file(hex_path, 0, &error)) {
        std::cerr << error << "\n";
        return 1;
    }

    OoOCpu cpu(memory);
    cpu.reset(static_cast<u32>(start_pc64));
    // trace_json 会隐含开启 trace，用于前端或脚本消费事件流。
    cpu.set_trace(trace);
    cpu.set_trace_json(trace_json);

    const bool ok = cpu.run(max_cycles, &error);
    if (stats_json) {
        // JSON 模式只输出统计，避免寄存器转储影响脚本解析。
        // 这里手工拼 JSON，字段全是数值/布尔可控内容，不涉及字符串转义问题。
        std::cout << "{\"stats\":{"
                  << "\"cycles\":" << cpu.cycles()
                  << ",\"committed_insts\":" << cpu.committed_insts()
                  << ",\"ipc\":" << cpu.ipc()
                  << ",\"branch_predictions\":" << cpu.branch_predictions()
                  << ",\"branch_mispredictions\":" << cpu.branch_mispredictions()
                  << ",\"flushes\":" << cpu.flushes()
                  << ",\"rs_stalls\":" << cpu.rs_stalls()
                  << ",\"rob_stalls\":" << cpu.rob_stalls()
                  << ",\"store_commits\":" << cpu.store_commits()
                  << ",\"icache_hits\":" << cpu.icache_hits()
                  << ",\"icache_misses\":" << cpu.icache_misses()
                  << ",\"icache_hit_rate\":" << cpu.icache_hit_rate()
                  << "}}\n";
    } else {
        std::cout << "cycles: " << cpu.cycles() << "\n";
        std::cout << "pc: 0x" << std::hex << cpu.pc() << std::dec << "\n";
        std::cout << "committed_insts: " << cpu.committed_insts() << "\n";
        std::cout << "ipc: " << cpu.ipc() << "\n";
        std::cout << "branch_predictions: " << cpu.branch_predictions() << "\n";
        std::cout << "branch_mispredictions: " << cpu.branch_mispredictions() << "\n";
        std::cout << "flushes: " << cpu.flushes() << "\n";
        std::cout << "rs_stalls: " << cpu.rs_stalls() << "\n";
        std::cout << "rob_stalls: " << cpu.rob_stalls() << "\n";
        std::cout << "store_commits: " << cpu.store_commits() << "\n";
        std::cout << "icache_hits: " << cpu.icache_hits() << "\n";
        std::cout << "icache_misses: " << cpu.icache_misses() << "\n";
        std::cout << "icache_hit_rate: " << cpu.icache_hit_rate() << "\n";
        cpu.dump_registers(std::cout);
    }

    if (!ok) {
        std::cerr << error << "\n";
        return 1;
    }
    return 0;
}
