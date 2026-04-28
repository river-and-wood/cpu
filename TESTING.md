# 测试说明

运行：

```bash
./run_tests.sh
```

当前测试结果：

```text
PASS: all CPU core tests
```

## `riscv_sim` 已测试指令

- 算术：`ADDI`, `ADD`, `SUB`
- 逻辑：`AND`, `ANDI`, `OR`, `ORI`, `XOR`, `XORI`
- 移位：`SLL`, `SLLI`, `SRL`, `SRLI`, `SRA`, `SRAI`
- 比较：`SLT`, `SLTI`, `SLTU`, `SLTIU`
- Load/Store：`LB`, `LBU`, `LH`, `LHU`, `LW`, `SB`, `SH`, `SW`
- 分支：`BEQ`, `BNE`, `BLT`, `BGE`, `BLTU`, `BGEU`
- 跳转：`JAL`, `JALR`
- 高位立即数：`LUI`, `AUIPC`
- 实验扩展：`MUL`, `DIV`
- 停机：`ECALL`
- 内存映射停机：写 `0x30004`

## 测试文件

- `tests/arith.hex`
- `tests/memory.hex`
- `tests/branch.hex`
- `tests/logic_shift_cmp.hex`
- `tests/load_sizes.hex`
- `tests/branch_all.hex`
- `tests/jump_auipc.hex`
- `tests/mul_div.hex`
- `tests/ooo_raw.hex`
- `tests/ooo_waw.hex`
- `tests/ooo_parallel.hex`
- `tests/edge_negative.hex`
- `tests/edge_x0.hex`
- `tests/edge_branch_not_taken.hex`
- `tests/edge_signext.hex`
- `tests/edge_jalr.hex`
- `tests/edge_cache_loop.hex`
- `tests/edge_rs_stall.hex`

同一批核心测试也会跑 `ooo_sim`，用于验证乱序/保留站/tag/cache 版本和顺序功能版结果一致。
