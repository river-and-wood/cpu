#include "ooo_cpu.hpp"
#include <cstddef>

namespace riscv_sim {

// icache.cpp 实现取指路径上的极简两路组相联 I-Cache。
// 它只缓存 32 位指令字，用于展示 hit/miss 统计，不建模真实访存延迟。
OoOCpu::InstructionCache::InstructionCache()
    // 两个 way 都有 128 行；replace_way_ 记录下一次 miss 优先替换哪一路。
    : way0_(128), way1_(128), replace_way_(128, false), hits_(0), misses_(0) {}
void OoOCpu::InstructionCache::reset() {
    for (std::size_t i = 0; i < way0_.size(); ++i) {
        way0_[i] = Line();
        way1_[i] = Line();
        replace_way_[i] = false;
    }
    hits_ = 0;
    misses_ = 0;
}
u32 OoOCpu::InstructionCache::fetch(Memory &memory, u32 pc) {
    // 128 组、2 路组相联；PC 按 4 字节对齐，所以用 pc[8:2] 做 index。
    const u32 index = (pc >> 2) & 0x7fu;
    // tag 使用 PC 高位，区分映射到同一 index 的不同指令地址。
    const u32 tag = pc >> 9;
    if (way0_[index].valid && way0_[index].tag == tag) {
        ++hits_;
        return way0_[index].data;
    }
    if (way1_[index].valid && way1_[index].tag == tag) {
        ++hits_;
        return way1_[index].data;
    }

    ++misses_;
    // miss 时从 Memory 读取 32 位指令，并填入其中一路。
    Line line;
    line.valid = true;
    line.tag = tag;
    line.data = memory.read32(pc);
    if (!way0_[index].valid || !replace_way_[index]) way0_[index] = line;
    else way1_[index] = line;
    // 每次填充后翻转替换位，形成简单的近似轮换策略。
    replace_way_[index] = !replace_way_[index];
    return line.data;
}

u64 OoOCpu::InstructionCache::hits() const { return hits_; }
u64 OoOCpu::InstructionCache::misses() const { return misses_; }

} // namespace riscv_sim
