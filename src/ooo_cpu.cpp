#include "ooo_cpu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace riscv_sim {

namespace {

const std::size_t ALU_RS_CAP = 8;
const std::size_t LS_RS_CAP = 8;
const std::size_t BR_RS_CAP = 4;

bool writes_register(const DecodedInst &inst) {
    switch (inst.kind) {
    case OpcodeKind::Lui:
    case OpcodeKind::Auipc:
    case OpcodeKind::Jal:
    case OpcodeKind::Jalr:
    case OpcodeKind::Load:
    case OpcodeKind::OpImm:
    case OpcodeKind::Op:
        return inst.rd != 0;
    default:
        return false;
    }
}

u32 load_value(Memory &mem, const DecodedInst &inst, u32 addr) {
    switch (inst.funct3) {
    case 0x0: return static_cast<u32>(sign_extend(mem.read8(addr), 8));
    case 0x1: return static_cast<u32>(sign_extend(mem.read16(addr), 16));
    case 0x2: return mem.read32(addr);
    case 0x4: return mem.read8(addr);
    case 0x5: return mem.read16(addr);
    default: return 0;
    }
}

bool store_value(Memory &mem, const DecodedInst &inst, u32 addr, u32 value) {
    switch (inst.funct3) {
    case 0x0: mem.write8(addr, static_cast<u8>(value)); return true;
    case 0x1: mem.write16(addr, static_cast<u16>(value)); return true;
    case 0x2: mem.write32(addr, value); return true;
    default: return false;
    }
}

} // namespace

OoOCpu::InstructionCache::InstructionCache()
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
    const u32 index = (pc >> 2) & 0x7fu;
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
    Line line;
    line.valid = true;
    line.tag = tag;
    line.data = memory.read32(pc);
    if (!way0_[index].valid || !replace_way_[index]) way0_[index] = line;
    else way1_[index] = line;
    replace_way_[index] = !replace_way_[index];
    return line.data;
}

u64 OoOCpu::InstructionCache::hits() const { return hits_; }
u64 OoOCpu::InstructionCache::misses() const { return misses_; }

OoOCpu::OoOCpu(Memory &memory) : mem_(memory) {
    clear();
}

void OoOCpu::clear() {
    for (unsigned i = 0; i < 32; ++i) regs_[i] = RegSlot();
    pc_ = 0;
    cycles_ = 0;
    next_tag_ = 1;
    fetch_stalled_ = false;
    halt_when_empty_ = false;
    trace_enabled_ = false;
    alu_rs_.clear();
    ls_rs_.clear();
    branch_rs_.clear();
    alu_fu_[0] = ExecSlot();
    alu_fu_[1] = ExecSlot();
    ls_fu_ = ExecSlot();
    branch_fu_ = ExecSlot();
}

void OoOCpu::reset(u32 pc) {
    clear();
    pc_ = pc;
    icache_.reset();
    mem_.set_cycles(0);
}

void OoOCpu::set_trace(bool enabled) {
    trace_enabled_ = enabled;
}

bool OoOCpu::step(std::string *error) {
    if (mem_.stopped()) return false;

    if (trace_enabled_) trace_cycle_begin();

    std::vector<WriteBack> wbs;
    Redirect redirect;
    complete_exec_slots(&wbs, &redirect);
    broadcast(wbs);

    if (redirect.valid) {
        if (trace_enabled_) trace_redirect(redirect.pc);
        pc_ = redirect.pc;
        fetch_stalled_ = false;
    }

    start_ready_ops();

    if (!halt_when_empty_ && !fetch_stalled_) {
        if (!allocate_one(error)) return false;
    } else if (trace_enabled_ && fetch_stalled_) {
        trace_stall("控制指令未决，暂停取指");
    }

    ++cycles_;
    mem_.set_cycles(cycles_);

    if (halt_when_empty_ && is_empty()) {
        if (trace_enabled_) std::cout << "  停机: ecall 已到达且乱序队列已排空\n";
        mem_.write8(IO_TIMER, 0);
    }
    return !mem_.stopped();
}

bool OoOCpu::run(u64 max_cycles, std::string *error) {
    for (u64 i = 0; i < max_cycles; ++i) {
        if (!step(error)) return mem_.stopped();
    }
    if (error) *error = "cycle limit reached";
    return false;
}

bool OoOCpu::is_empty() const {
    return alu_rs_.empty() && ls_rs_.empty() && branch_rs_.empty() &&
           !alu_fu_[0].busy && !alu_fu_[1].busy && !ls_fu_.busy && !branch_fu_.busy;
}

bool OoOCpu::allocate_one(std::string *error) {
    const u32 inst_word = icache_.fetch(mem_, pc_);
    const DecodedInst inst = decode(inst_word);

    if (inst.kind == OpcodeKind::System) {
        if (inst.raw == 0x00000073u) {
            halt_when_empty_ = true;
            if (trace_enabled_) {
                std::cout << "  取指: pc=0x" << std::hex << pc_ << " inst=0x" << inst_word
                          << std::dec << " -> ecall，等待流水线排空\n";
            }
            pc_ += 4;
            return true;
        }
        if (error) *error = "unsupported system instruction";
        return false;
    }

    MicroOp op;
    if (!decode_to_micro_op(inst, pc_, &op, error)) return false;
    if (!enqueue_rs(op)) {
        if (trace_enabled_) trace_stall("目标保留站已满，分配暂停并重试当前 PC");
        return true; // 保留站满，当前周期停顿，下一周期重试同一 PC。
    }

    if (trace_enabled_) trace_allocate(op);

    if (writes_register(inst)) {
        regs_[inst.rd].tag = op.dst_tag; // 目标寄存器加锁：后续读者等待该 tag。
    }

    // 控制流指令暂停继续取指，直到执行单元给出真实下一 PC。
    if (inst.kind == OpcodeKind::Branch || inst.kind == OpcodeKind::Jal || inst.kind == OpcodeKind::Jalr)
        fetch_stalled_ = true;
    else
        pc_ += 4;
    return true;
}

bool OoOCpu::decode_to_micro_op(const DecodedInst &inst, u32 pc, MicroOp *out, std::string *error) {
    MicroOp op;
    op.valid = true;
    op.inst = inst;
    op.pc = pc;
    op.rd = inst.rd;
    op.op = static_cast<OpKind>(inst.kind);
    op.src1.ready = true;
    op.src2.ready = true;

    switch (inst.kind) {
    case OpcodeKind::Lui:
    case OpcodeKind::Auipc:
        op.unit = UnitKind::Alu;
        break;
    case OpcodeKind::Jal:
        op.unit = UnitKind::Alu;
        break;
    case OpcodeKind::Jalr:
        op.unit = UnitKind::Alu;
        op.src1 = read_operand(inst.rs1);
        break;
    case OpcodeKind::Branch:
        op.unit = UnitKind::Branch;
        op.src1 = read_operand(inst.rs1);
        op.src2 = read_operand(inst.rs2);
        break;
    case OpcodeKind::Load:
        op.unit = UnitKind::LoadStore;
        op.src1 = read_operand(inst.rs1);
        break;
    case OpcodeKind::Store:
        op.unit = UnitKind::LoadStore;
        op.src1 = read_operand(inst.rs1);
        op.src2 = read_operand(inst.rs2);
        break;
    case OpcodeKind::OpImm:
        op.unit = UnitKind::Alu;
        op.src1 = read_operand(inst.rs1);
        break;
    case OpcodeKind::Op:
        op.unit = UnitKind::Alu;
        op.src1 = read_operand(inst.rs1);
        op.src2 = read_operand(inst.rs2);
        break;
    default:
        if (error) {
            std::ostringstream out_msg;
            out_msg << "unsupported instruction at pc=0x" << std::hex << pc
                    << " inst=0x" << inst.raw << " (" << disassemble(inst) << ")";
            *error = out_msg.str();
        }
        return false;
    }

    if (writes_register(inst)) op.dst_tag = next_tag_++;
    *out = op;
    return true;
}

OoOCpu::Operand OoOCpu::read_operand(u8 reg) const {
    Operand op;
    if (reg == 0) {
        op.ready = true;
        op.value = 0;
        op.tag = 0;
        return op;
    }
    if (regs_[reg].tag == 0) {
        op.ready = true;
        op.value = regs_[reg].value;
        op.tag = 0;
    } else {
        op.ready = false;
        op.value = 0;
        op.tag = regs_[reg].tag;
    }
    return op;
}

bool OoOCpu::enqueue_rs(const MicroOp &op) {
    if (op.unit == UnitKind::Alu) {
        if (alu_rs_.size() >= ALU_RS_CAP) return false;
        alu_rs_.push_back(op);
        return true;
    }
    if (op.unit == UnitKind::LoadStore) {
        if (ls_rs_.size() >= LS_RS_CAP) return false;
        ls_rs_.push_back(op);
        return true;
    }
    if (branch_rs_.size() >= BR_RS_CAP) return false;
    branch_rs_.push_back(op);
    return true;
}

bool OoOCpu::src_ready(const MicroOp &op) const {
    return op.src1.ready && op.src2.ready;
}

unsigned OoOCpu::latency(const MicroOp &op) const {
    if (op.unit == UnitKind::LoadStore) return 2;
    if (op.unit == UnitKind::Branch) return 1;
    if (op.inst.kind == OpcodeKind::Op && op.inst.funct7 == 0x01 && op.inst.funct3 == 0x0) return 3;  // MUL
    if (op.inst.kind == OpcodeKind::Op && op.inst.funct7 == 0x01 && op.inst.funct3 == 0x4) return 35; // DIV
    return 1;
}

void OoOCpu::start_ready_ops() {
    for (unsigned fu = 0; fu < 2; ++fu) {
        if (alu_fu_[fu].busy) continue;
        for (std::size_t i = 0; i < alu_rs_.size(); ++i) {
            if (!src_ready(alu_rs_[i])) continue;
            alu_fu_[fu].busy = true;
            alu_fu_[fu].remain = latency(alu_rs_[i]);
            alu_fu_[fu].op = alu_rs_[i];
            if (trace_enabled_) trace_start(alu_fu_[fu], fu);
            erase_started(&alu_rs_, i);
            break;
        }
    }

    if (!ls_fu_.busy) {
        for (std::size_t i = 0; i < ls_rs_.size(); ++i) {
            if (!src_ready(ls_rs_[i])) continue;
            ls_fu_.busy = true;
            ls_fu_.remain = latency(ls_rs_[i]);
            ls_fu_.op = ls_rs_[i];
            if (trace_enabled_) trace_start(ls_fu_, 2);
            erase_started(&ls_rs_, i);
            break;
        }
    }

    if (!branch_fu_.busy) {
        for (std::size_t i = 0; i < branch_rs_.size(); ++i) {
            if (!src_ready(branch_rs_[i])) continue;
            branch_fu_.busy = true;
            branch_fu_.remain = latency(branch_rs_[i]);
            branch_fu_.op = branch_rs_[i];
            if (trace_enabled_) trace_start(branch_fu_, 3);
            erase_started(&branch_rs_, i);
            break;
        }
    }
}

void OoOCpu::erase_started(std::vector<MicroOp> *rs, std::size_t index) {
    rs->erase(rs->begin() + static_cast<std::ptrdiff_t>(index));
}

void OoOCpu::complete_exec_slots(std::vector<WriteBack> *wbs, Redirect *redirect) {
    ExecSlot *slots[] = { &alu_fu_[0], &alu_fu_[1], &ls_fu_, &branch_fu_ };
    for (unsigned i = 0; i < 4; ++i) {
        ExecSlot *slot = slots[i];
        if (!slot->busy) continue;
        if (slot->remain > 0) --slot->remain;
        if (slot->remain != 0) continue;

        WriteBack wb;
        Redirect local_redirect;
        std::string ignored;
        execute_result(slot->op, &wb, &local_redirect, &ignored);
        if (wb.valid) wbs->push_back(wb);
        if (trace_enabled_ && wb.valid) trace_writeback(wb);
        if (local_redirect.valid) *redirect = local_redirect;
        slot->busy = false;
    }
}

bool OoOCpu::execute_result(const MicroOp &op, WriteBack *wb, Redirect *redirect, std::string *error) {
    const DecodedInst &inst = op.inst;
    wb->valid = false;
    wb->tag = op.dst_tag;
    wb->rd = op.rd;

    switch (inst.kind) {
    case OpcodeKind::Lui:
        wb->valid = op.dst_tag != 0;
        wb->value = static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Auipc:
        wb->valid = op.dst_tag != 0;
        wb->value = op.pc + static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Jal:
        wb->valid = op.dst_tag != 0;
        wb->value = op.pc + 4;
        redirect->valid = true;
        redirect->pc = op.pc + static_cast<u32>(inst.imm);
        break;
    case OpcodeKind::Jalr:
        wb->valid = op.dst_tag != 0;
        wb->value = op.pc + 4;
        redirect->valid = true;
        redirect->pc = (op.src1.value + static_cast<u32>(inst.imm)) & ~1u;
        break;
    case OpcodeKind::Branch: {
        const u32 x = op.src1.value;
        const u32 y = op.src2.value;
        bool take = false;
        switch (inst.funct3) {
        case 0x0: take = (x == y); break;
        case 0x1: take = (x != y); break;
        case 0x4: take = (as_s32(x) < as_s32(y)); break;
        case 0x5: take = (as_s32(x) >= as_s32(y)); break;
        case 0x6: take = (x < y); break;
        case 0x7: take = (x >= y); break;
        default: if (error) *error = "unsupported branch"; return false;
        }
        redirect->valid = true;
        redirect->pc = take ? op.pc + static_cast<u32>(inst.imm) : op.pc + 4;
        break;
    }
    case OpcodeKind::Load: {
        const u32 addr = op.src1.value + static_cast<u32>(inst.imm);
        wb->valid = op.dst_tag != 0;
        wb->value = load_value(mem_, inst, addr);
        break;
    }
    case OpcodeKind::Store: {
        const u32 addr = op.src1.value + static_cast<u32>(inst.imm);
        if (!store_value(mem_, inst, addr, op.src2.value)) {
            if (error) *error = "unsupported store";
            return false;
        }
        break;
    }
    case OpcodeKind::OpImm: {
        const u32 x = op.src1.value;
        const u32 shamt = (inst.raw >> 20) & 0x1fu;
        wb->valid = op.dst_tag != 0;
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
        wb->valid = op.dst_tag != 0;
        switch (inst.funct3) {
        case 0x0:
            if (inst.funct7 == 0x00) wb->value = x + y;
            else if (inst.funct7 == 0x20) wb->value = x - y;
            else if (inst.funct7 == 0x01) wb->value = x * y;
            else return false;
            break;
        case 0x1: wb->value = x << shamt; break;
        case 0x2: wb->value = as_s32(x) < as_s32(y) ? 1u : 0u; break;
        case 0x3: wb->value = x < y ? 1u : 0u; break;
        case 0x4:
            if (inst.funct7 == 0x01) {
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

void OoOCpu::broadcast(const std::vector<WriteBack> &wbs) {
    for (std::size_t i = 0; i < wbs.size(); ++i) {
        const WriteBack &wb = wbs[i];
        apply_writeback(wb);
        for (std::size_t j = 0; j < alu_rs_.size(); ++j) {
            update_waiting_operand(&alu_rs_[j].src1, wb);
            update_waiting_operand(&alu_rs_[j].src2, wb);
        }
        for (std::size_t j = 0; j < ls_rs_.size(); ++j) {
            update_waiting_operand(&ls_rs_[j].src1, wb);
            update_waiting_operand(&ls_rs_[j].src2, wb);
        }
        for (std::size_t j = 0; j < branch_rs_.size(); ++j) {
            update_waiting_operand(&branch_rs_[j].src1, wb);
            update_waiting_operand(&branch_rs_[j].src2, wb);
        }
    }
}

void OoOCpu::apply_writeback(const WriteBack &wb) {
    if (!wb.valid || wb.rd == 0) return;
    // WAW 保护：只有寄存器当前 tag 仍等于写回 tag，结果才允许落入架构寄存器。
    if (regs_[wb.rd].tag == wb.tag) {
        regs_[wb.rd].value = wb.value;
        regs_[wb.rd].tag = 0;
    }
}

void OoOCpu::update_waiting_operand(Operand *operand, const WriteBack &wb) {
    if (!wb.valid || operand->ready || operand->tag != wb.tag) return;
    operand->ready = true;
    operand->value = wb.value;
    operand->tag = 0;
}

void OoOCpu::dump_registers(std::ostream &out) const {
    out << std::hex << std::setfill('0');
    for (unsigned i = 0; i < 32; ++i) {
        out << "x" << std::dec << std::setw(2) << std::setfill(' ') << i
            << std::setfill('0') << "=0x" << std::hex << std::setw(8)
            << regs_[i].value << ((i % 4 == 3) ? '\n' : ' ');
    }
    out << std::dec << std::setfill(' ');
}

u32 OoOCpu::reg(unsigned index) const {
    return index < 32 ? regs_[index].value : 0;
}

u32 OoOCpu::pc() const { return pc_; }
u64 OoOCpu::cycles() const { return cycles_; }
std::size_t OoOCpu::alu_rs_size() const { return alu_rs_.size(); }
std::size_t OoOCpu::ls_rs_size() const { return ls_rs_.size(); }
std::size_t OoOCpu::branch_rs_size() const { return branch_rs_.size(); }
u64 OoOCpu::icache_hits() const { return icache_.hits(); }
u64 OoOCpu::icache_misses() const { return icache_.misses(); }

const char *OoOCpu::unit_name(UnitKind unit) const {
    switch (unit) {
    case UnitKind::Alu: return "ALU";
    case UnitKind::LoadStore: return "LS";
    case UnitKind::Branch: return "BR";
    }
    return "?";
}

void OoOCpu::trace_cycle_begin() const {
    std::cout << "[cycle " << cycles_ << "] pc=0x" << std::hex << pc_ << std::dec
              << " | ALU_RS=" << alu_rs_.size()
              << " LS_RS=" << ls_rs_.size()
              << " BR_RS=" << branch_rs_.size()
              << " | FU{alu0=" << (alu_fu_[0].busy ? "busy" : "idle")
              << ", alu1=" << (alu_fu_[1].busy ? "busy" : "idle")
              << ", ls=" << (ls_fu_.busy ? "busy" : "idle")
              << ", br=" << (branch_fu_.busy ? "busy" : "idle")
              << "}\n";
}

void OoOCpu::trace_allocate(const MicroOp &op) const {
    std::cout << "  分配: pc=0x" << std::hex << op.pc
              << " inst=0x" << op.inst.raw << std::dec
              << " -> " << unit_name(op.unit);
    if (op.dst_tag != 0)
        std::cout << " rd=x" << unsigned(op.rd) << " tag=T" << op.dst_tag;
    if (!op.src1.ready)
        std::cout << " src1等待T" << op.src1.tag;
    if (!op.src2.ready)
        std::cout << " src2等待T" << op.src2.tag;
    std::cout << "\n";
}

void OoOCpu::trace_stall(const char *reason) const {
    std::cout << "  停顿: " << reason << "\n";
}

void OoOCpu::trace_start(const ExecSlot &slot, unsigned fu_index) const {
    const char *fu_name = (fu_index == 0) ? "ALU0" :
                          (fu_index == 1) ? "ALU1" :
                          (fu_index == 2) ? "LS" : "BR";
    std::cout << "  发射执行: " << fu_name
              << " pc=0x" << std::hex << slot.op.pc
              << " inst=0x" << slot.op.inst.raw << std::dec
              << " latency=" << slot.remain << "\n";
}

void OoOCpu::trace_writeback(const WriteBack &wb) const {
    std::cout << "  写回广播: T" << wb.tag
              << " -> x" << unsigned(wb.rd)
              << " = 0x" << std::hex << wb.value << std::dec << "\n";
}

void OoOCpu::trace_redirect(u32 pc) const {
    std::cout << "  控制重定向: next_pc=0x" << std::hex << pc << std::dec << "\n";
}

} // namespace riscv_sim
