#!/usr/bin/env bash
set -eu

fail() {
  echo "FAIL: $1" >&2
  exit 1
}

check_contains() {
  haystack="$1"
  needle="$2"
  name="$3"
  printf '%s\n' "$haystack" | grep -q "$needle" || fail "$name expected '$needle'"
}

make >/dev/null

out="$(./riscv_sim tests/programs/arith.hex)"
check_contains "$out" "x 3=0x0000001e" "arith x3"
check_contains "$out" "x 4=0x0000000a" "arith x4"

out="$(./riscv_sim tests/programs/memory.hex)"
check_contains "$out" "x 1=0x0000002a" "memory x1"
check_contains "$out" "x 2=0x0000002a" "memory x2"

out="$(./riscv_sim tests/programs/branch.hex)"
check_contains "$out" "x 3=0x00000009" "branch x3"

out="$(./riscv_sim tests/programs/logic_shift_cmp.hex)"
check_contains "$out" "x10=0x00000000" "logic and"
check_contains "$out" "x11=0x00000002" "logic andi"
check_contains "$out" "x12=0x0000001e" "logic or"
check_contains "$out" "x13=0x0000000f" "logic ori"
check_contains "$out" "x14=0x0000001e" "logic xor"
check_contains "$out" "x15=0x0000000d" "logic xori"
check_contains "$out" "x16=0x00000280" "shift sll"
check_contains "$out" "x17=0x00000050" "shift slli"
check_contains "$out" "x18=0x00000000" "shift srl"
check_contains "$out" "x19=0x00000002" "shift srli"
check_contains "$out" "x20=0x00000000" "shift sra"
check_contains "$out" "x21=0x00000002" "shift srai"
check_contains "$out" "x22=0x00000001" "cmp slt"
check_contains "$out" "x23=0x00000001" "cmp slti"
check_contains "$out" "x24=0x00000001" "cmp sltu"
check_contains "$out" "x25=0x00000001" "cmp sltiu"

out="$(./riscv_sim tests/programs/load_sizes.hex)"
check_contains "$out" "x 2=0xffffffff" "load lb"
check_contains "$out" "x 3=0x000000ff" "load lbu"
check_contains "$out" "x 4=0xffffffff" "load lh"
check_contains "$out" "x 6=0x0000ffff" "load lhu"
check_contains "$out" "x 8=0x00000034" "store sb"
check_contains "$out" "x10=0x00000123" "store sh"

out="$(./riscv_sim tests/programs/branch_all.hex)"
check_contains "$out" "x10=0x00000009" "branch beq"
check_contains "$out" "x11=0x00000009" "branch bne"
check_contains "$out" "x12=0x00000009" "branch blt"
check_contains "$out" "x13=0x00000009" "branch bge"
check_contains "$out" "x14=0x00000009" "branch bltu"
check_contains "$out" "x15=0x00000009" "branch bgeu"

out="$(./riscv_sim tests/programs/jump_auipc.hex)"
check_contains "$out" "x 1=0x00000009" "jump jal path"
check_contains "$out" "x 3=0x00000009" "jump jalr path"
check_contains "$out" "x 5=0x00000004" "jump jal link"
check_contains "$out" "x 6=0x00000014" "jump jalr link"
check_contains "$out" "x 7=0x0000101c" "auipc"

out="$(./riscv_sim tests/programs/mul_div.hex)"
check_contains "$out" "x 3=0x000000fc" "mul"
check_contains "$out" "x 4=0x00000007" "div"

out="$(./riscv_sim tests/programs/tomasulo_rv32.hex)"
check_contains "$out" "x 6=0x00000009" "tomasulo rv32 final x6"
check_contains "$out" "x10=0x0000000c" "tomasulo rv32 mul"
check_contains "$out" "x18=0x00000006" "tomasulo rv32 sub"
check_contains "$out" "x20=0x00000001" "tomasulo rv32 div"

out="$(./ooo_sim tests/programs/arith.hex)"
check_contains "$out" "x 3=0x0000001e" "ooo arith x3"
check_contains "$out" "x 4=0x0000000a" "ooo arith x4"

out="$(./ooo_sim tests/programs/memory.hex)"
check_contains "$out" "x 1=0x0000002a" "ooo memory x1"
check_contains "$out" "x 2=0x0000002a" "ooo memory x2"

out="$(./ooo_sim tests/programs/branch_all.hex)"
check_contains "$out" "x10=0x00000009" "ooo branch beq"
check_contains "$out" "x11=0x00000009" "ooo branch bne"
check_contains "$out" "x12=0x00000009" "ooo branch blt"
check_contains "$out" "x13=0x00000009" "ooo branch bge"
check_contains "$out" "x14=0x00000009" "ooo branch bltu"
check_contains "$out" "x15=0x00000009" "ooo branch bgeu"

out="$(./ooo_sim tests/programs/logic_shift_cmp.hex)"
check_contains "$out" "x10=0x00000000" "ooo logic and"
check_contains "$out" "x16=0x00000280" "ooo shift sll"
check_contains "$out" "x25=0x00000001" "ooo cmp sltiu"

out="$(./ooo_sim tests/programs/load_sizes.hex)"
check_contains "$out" "x 2=0xffffffff" "ooo load lb"
check_contains "$out" "x10=0x00000123" "ooo store sh"

out="$(./ooo_sim tests/programs/jump_auipc.hex)"
check_contains "$out" "x 1=0x00000009" "ooo jump jal path"
check_contains "$out" "x 7=0x0000101c" "ooo auipc"

out="$(./ooo_sim tests/programs/mul_div.hex)"
check_contains "$out" "x 3=0x000000fc" "ooo mul"
check_contains "$out" "x 4=0x00000007" "ooo div"

out="$(./ooo_sim tests/programs/tomasulo_rv32.hex)"
check_contains "$out" "x 6=0x00000009" "ooo tomasulo rv32 final x6"
check_contains "$out" "x10=0x0000000c" "ooo tomasulo rv32 mul"
check_contains "$out" "x18=0x00000006" "ooo tomasulo rv32 sub"
check_contains "$out" "x20=0x00000001" "ooo tomasulo rv32 div"

out="$(./ooo_sim tests/programs/ooo_raw.hex)"
check_contains "$out" "x 1=0x00000005" "ooo raw x1"
check_contains "$out" "x 2=0x0000000a" "ooo raw x2"
check_contains "$out" "x 3=0x0000000f" "ooo raw x3"

out="$(./ooo_sim tests/programs/ooo_waw.hex)"
check_contains "$out" "x 1=0x00000009" "ooo waw latest x1"
check_contains "$out" "x 2=0x00000012" "ooo waw dependent x2"

out="$(./ooo_sim tests/programs/ooo_war.hex)"
check_contains "$out" "x 1=0x00000009" "ooo war younger write x1"
check_contains "$out" "x 4=0x00000008" "ooo war older read preserved"
check_contains "$out" "x 5=0x00000011" "ooo war dependent latest x1"

out="$(./ooo_sim tests/programs/ooo_parallel.hex)"
check_contains "$out" "x 3=0x0000002a" "ooo parallel mul"
check_contains "$out" "x 4=0x00000003" "ooo parallel independent add"
check_contains "$out" "x 7=0x0000002d" "ooo parallel dependent add"

for sim in ./riscv_sim ./ooo_sim; do
  out="$($sim tests/programs/edge_negative.hex)"
  check_contains "$out" "x 3=0x00000001" "$sim edge slt signed"
  check_contains "$out" "x 4=0x00000000" "$sim edge sltu unsigned"
  check_contains "$out" "x 5=0xffffffff" "$sim edge sra"
  check_contains "$out" "x 6=0x7fffffff" "$sim edge srli"
  check_contains "$out" "x 7=0xffffffff" "$sim edge div negative"
  check_contains "$out" "x 8=0xffffffff" "$sim edge div zero"
  check_contains "$out" "x11=0x80000000" "$sim edge div overflow"

  out="$($sim tests/programs/edge_x0.hex)"
  check_contains "$out" "x 0=0x00000000" "$sim edge x0"
  check_contains "$out" "x 2=0x00000000" "$sim edge x0 dependent"

  out="$($sim tests/programs/edge_branch_not_taken.hex)"
  check_contains "$out" "x 3=0x00000007" "$sim edge beq not taken"
  check_contains "$out" "x 4=0x00000008" "$sim edge bne not taken"

  out="$($sim tests/programs/edge_signext.hex)"
  check_contains "$out" "x 1=0xfffff800" "$sim edge imm -2048"
  check_contains "$out" "x 2=0x000007ff" "$sim edge imm 2047"
  check_contains "$out" "x 3=0x000007fe" "$sim edge addi -1"
  check_contains "$out" "x 4=0x000007ff" "$sim edge andi -1"
  check_contains "$out" "x 5=0xffffffff" "$sim edge ori -1"

  out="$($sim tests/programs/edge_jalr.hex)"
  check_contains "$out" "x 2=0x00000000" "$sim edge jalr skip"
  check_contains "$out" "x 4=0x00000007" "$sim edge jalr target"
  check_contains "$out" "x 5=0x00000008" "$sim edge jalr link"
done

out="$(./ooo_sim tests/programs/edge_cache_loop.hex)"
check_contains "$out" "x 1=0x00000000" "ooo cache loop result"
check_contains "$out" "icache_hits:" "ooo cache hits line"
check_contains "$out" "committed_insts:" "ooo committed stats line"
check_contains "$out" "ipc:" "ooo ipc stats line"

out="$(./ooo_sim tests/programs/edge_rs_stall.hex)"
check_contains "$out" "x12=0x0000000c" "ooo rs stall retry"

out="$(./ooo_sim tests/programs/branch_all.hex --stats-json)"
check_contains "$out" "\"branch_predictions\":" "ooo stats json branch predictions"
check_contains "$out" "\"branch_mispredictions\":" "ooo stats json branch misses"

out="$(./ooo_sim tests/programs/edge_branch_not_taken.hex --trace-json --max-cycles 12)"
check_contains "$out" "\"event\":\"cycle\"" "ooo trace json cycle"
check_contains "$out" "\"event\":\"commit\"" "ooo trace json commit"

out="$(./scripts/benchmark_report.sh)"
check_contains "$out" "| workload | riscv_sim cycles | ooo_sim cycles | delta |" "benchmark report header"
check_contains "$out" "tests/programs/ooo_parallel.hex" "benchmark report ooo parallel"

echo "PASS: all CPU core tests"
