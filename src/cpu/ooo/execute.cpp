#include "ooo_cpu.hpp"
#include "ooo_internal.hpp"

namespace riscv_sim {

using namespace ooo_detail;

// execute.cpp 只描述微操作执行语义：输入来自保留站操作数，输出为 WriteBack/Redirect。
// Store 在这里仅计算地址和数据，真正写内存要等 rob.cpp 中的按序提交阶段。
bool OoOCpu::execute_result(const MicroOp &op, WriteBack *wb, Redirect *redirect, std::string *error) {
    const DecodedInst &inst = op.inst;
    // 默认认为该微操作会写回一个事件；具体是否有寄存器值由 has_value 表示。
    // 即使没有寄存器值，Branch/Store 也需要让 ROB 知道这条指令已经执行完成。
    wb->valid = true;
    wb->tag = op.dst_tag;
    wb->rd = op.rd;
    wb->has_value = false;
    wb->is_store = false;

    switch (inst.kind) {
    case OpcodeKind::Lui:
        // LUI 直接产生高 20 位立即数；写 x0 时 writes_register 返回 false。
        wb->has_value = writes_register(inst);
        wb->value = static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Auipc:
        wb->has_value = writes_register(inst);
        wb->value = op.pc + static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Jal: {
        // JAL 真实目标可以在执行阶段再次计算，并与前端预测 PC 对比。
        const u32 actual_pc = op.pc + static_cast<u32>(inst.imm);
        wb->has_value = writes_register(inst);
        wb->value = op.pc + 4;
        if (actual_pc != op.predicted_pc) {
            redirect->valid = true;
            redirect->tag = op.dst_tag;
            redirect->pc = actual_pc;
        }
        break;
    }
    case OpcodeKind::Jalr: {
        // JALR 的真实目标依赖 rs1，所以必须等源操作数 ready 后才能确认是否预测错误。
        const u32 actual_pc = (op.src1.value + static_cast<u32>(inst.imm)) & ~1u;
        wb->has_value = writes_register(inst);
        wb->value = op.pc + 4;
        if (actual_pc != op.predicted_pc) {
            redirect->valid = true;
            redirect->tag = op.dst_tag;
            redirect->pc = actual_pc;
        }
        break;
    }
    case OpcodeKind::Branch: {
        const u32 x = op.src1.value;
        const u32 y = op.src2.value;
        bool take = false;
        // 分支执行时才知道真实方向，与 allocate_one 中的静态预测比较。
        switch (inst.funct3) {
        case 0x0: take = (x == y); break;
        case 0x1: take = (x != y); break;
        case 0x4: take = (as_s32(x) < as_s32(y)); break;
        case 0x5: take = (as_s32(x) >= as_s32(y)); break;
        case 0x6: take = (x < y); break;
        case 0x7: take = (x >= y); break;
        default: if (error) *error = "unsupported branch"; return false;
        }
        const u32 actual_pc = take ? op.pc + static_cast<u32>(inst.imm) : op.pc + 4;
        if (actual_pc != op.predicted_pc) {
            redirect->valid = true;
            redirect->tag = op.dst_tag;
            redirect->pc = actual_pc;
        }
        break;
    }
    case OpcodeKind::Load: {
        const u32 addr = op.src1.value + static_cast<u32>(inst.imm);
        // Load 在执行完成时就读取内存并把值写回 ROB/保留站。
        wb->has_value = writes_register(inst);
        wb->value = load_value(mem_, inst, addr);
        break;
    }
    case OpcodeKind::Store: {
        const u32 addr = op.src1.value + static_cast<u32>(inst.imm);
        // Store 这里只计算地址和值，不能提前写内存；否则错误路径上的 Store 无法撤销。
        wb->is_store = true;
        wb->store_addr = addr;
        wb->store_value = op.src2.value;
        break;
    }
    case OpcodeKind::OpImm: {
        const u32 x = op.src1.value;
        const u32 shamt = (inst.raw >> 20) & 0x1fu;
        wb->has_value = writes_register(inst);
        // funct3 决定 I-type ALU 的具体运算，和顺序 CPU 中的语义保持一致。
        switch (inst.funct3) {
        case 0x0: wb->value = x + static_cast<u32>(inst.imm); break;
        case 0x2: wb->value = as_s32(x) < inst.imm ? 1u : 0u; break;
        case 0x3: wb->value = x < static_cast<u32>(inst.imm) ? 1u : 0u; break;
        case 0x4: wb->value = x ^ static_cast<u32>(inst.imm); break;
        case 0x6: wb->value = x | static_cast<u32>(inst.imm); break;
        case 0x7: wb->value = x & static_cast<u32>(inst.imm); break;
        case 0x1: wb->value = x << shamt; break;
        case 0x5: wb->value = (inst.funct7 == 0x20) ? static_cast<u32>(as_s32(x) >> shamt) : x >> shamt; break;
        default: if (error) *error = "unsupported opimm"; return false;
        }
        break;
    }
    case OpcodeKind::Op: {
        const u32 x = op.src1.value;
        const u32 y = op.src2.value;
        const u32 shamt = y & 0x1fu;
        wb->has_value = writes_register(inst);
        // R-type 的 ADD/SUB/MUL 共用 funct3=0，需要继续检查 funct7。
        switch (inst.funct3) {
        case 0x0:
            if (inst.funct7 == 0x00) wb->value = x + y;
            else if (inst.funct7 == 0x20) wb->value = x - y;
            else if (inst.funct7 == 0x01) wb->value = x * y;
            else { if (error) *error = "unsupported op"; return false; }
            break;
        case 0x1: wb->value = x << shamt; break;
        case 0x2: wb->value = as_s32(x) < as_s32(y) ? 1u : 0u; break;
        case 0x3: wb->value = x < y ? 1u : 0u; break;
        case 0x4:
            if (inst.funct7 == 0x01) {
                // DIV 的两个特殊情况按 RISC-V 规范处理：除零返回全 1，最小负数除 -1 返回自身。
                if (y == 0) wb->value = 0xffffffffu;
                else if (x == 0x80000000u && y == 0xffffffffu) wb->value = x;
                else wb->value = static_cast<u32>(as_s32(x) / as_s32(y));
            } else {
                wb->value = x ^ y;
            }
            break;
        case 0x5: wb->value = (inst.funct7 == 0x20) ? static_cast<u32>(as_s32(x) >> shamt) : x >> shamt; break;
        case 0x6: wb->value = x | y; break;
        case 0x7: wb->value = x & y; break;
        default: if (error) *error = "unsupported op"; return false;
        }
        break;
    }
    default:
        if (error) *error = "unsupported micro op";
        return false;
    }
    return true;
}

} // namespace riscv_sim
