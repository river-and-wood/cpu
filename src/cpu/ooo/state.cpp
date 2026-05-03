#include "ooo_cpu.hpp"
#include <iomanip>
#include <ostream>

namespace riscv_sim {

// state.cpp 集中放只读观察接口，命令行程序和测试通过这些函数读取架构状态和统计值。
void OoOCpu::dump_registers(std::ostream &out) const {
    // 这里只打印已经提交的架构值 regs_[i].value，不显示未提交的重命名 tag。
    out << std::hex << std::setfill('0');
    for (unsigned i = 0; i < 32; ++i) {
        out << "x" << std::dec << std::setw(2) << std::setfill(' ') << i
            << std::setfill('0') << "=0x" << std::hex << std::setw(8)
            << regs_[i].value << ((i % 4 == 3) ? '\n' : ' ');
    }
    out << std::dec << std::setfill(' ');
}
u32 OoOCpu::reg(unsigned index) const {
    // 越界读取返回 0，方便测试代码不用额外处理非法寄存器编号。
    return index < 32 ? regs_[index].value : 0;
}
u32 OoOCpu::pc() const { return pc_; }
u64 OoOCpu::cycles() const { return cycles_; }
u64 OoOCpu::committed_insts() const { return stats_.committed; }
u64 OoOCpu::branch_predictions() const { return stats_.branch_predictions; }
u64 OoOCpu::branch_mispredictions() const { return stats_.branch_mispredictions; }
u64 OoOCpu::flushes() const { return stats_.flushes; }
u64 OoOCpu::rs_stalls() const { return stats_.rs_stalls; }
u64 OoOCpu::rob_stalls() const { return stats_.rob_stalls; }
u64 OoOCpu::store_commits() const { return stats_.store_commits; }
double OoOCpu::ipc() const {
    // IPC = committed instructions / cycles；cycles 为 0 时避免除零。
    return cycles_ == 0 ? 0.0 : static_cast<double>(stats_.committed) / static_cast<double>(cycles_);
}
std::size_t OoOCpu::alu_rs_size() const { return alu_rs_.size(); }
std::size_t OoOCpu::ls_rs_size() const { return ls_rs_.size(); }
std::size_t OoOCpu::branch_rs_size() const { return branch_rs_.size(); }
std::size_t OoOCpu::rob_size() const { return rob_.size(); }
u64 OoOCpu::icache_hits() const { return icache_.hits(); }
u64 OoOCpu::icache_misses() const { return icache_.misses(); }
double OoOCpu::icache_hit_rate() const {
    const u64 total = icache_.hits() + icache_.misses();
    // 没有取指时命中率定义为 0，避免 NaN 出现在命令行/JSON 输出中。
    return total == 0 ? 0.0 : static_cast<double>(icache_.hits()) / static_cast<double>(total);
}

} // namespace riscv_sim
