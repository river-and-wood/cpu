#!/usr/bin/env bash
set -eu

make >/dev/null

cycle_of() {
  sim="$1"
  program="$2"
  "$sim" "$program" | awk '$1 == "cycles:" { print $2; exit }'
}

printf '| workload | riscv_sim cycles | ooo_sim cycles | delta |\n'
printf '|---|---:|---:|---:|\n'

for program in \
  tests/programs/ooo_parallel.hex \
  tests/programs/ooo_raw.hex \
  tests/programs/ooo_war.hex \
  tests/programs/ooo_waw.hex \
  tests/programs/memory.hex \
  tests/programs/branch_all.hex \
  tests/programs/mul_div.hex
do
  seq_cycles="$(cycle_of ./riscv_sim "$program")"
  ooo_cycles="$(cycle_of ./ooo_sim "$program")"
  delta=$((seq_cycles - ooo_cycles))
  printf '| %s | %s | %s | %+d |\n' "$program" "$seq_cycles" "$ooo_cycles" "$delta"
done
