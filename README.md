# C++11 CPU Core Project

这是从 Verilog CPU 项目中单独整理出来的 C++11 CPU 核心项目。

主目标只包括 CPU 指令执行本身，以及运行 CPU 所需的最小 RAM/停机机制。串口、HCI 调试连接、`fakecpu` 演示替身、FPGA 顶层外设等附加部分不作为本项目目标。

- `riscv_sim`：顺序功能级 CPU 核心模拟器，用于快速验证指令语义。
- `ooo_sim`：保留乱序/保留站/tag/cache 机制的 CPU 核心模拟器。

## 项目结构

```text
cpp_ooo_cpu_project/
├── src/          # C++11 CPU 源码
├── tests/        # 十六进制测试程序
├── build/        # 编译中间文件，由 make 生成，可删除
├── Makefile
├── run_tests.sh
├── README.md
└── TESTING.md
```

`riscv_sim` 保留了 Verilog CPU 核心中的主要行为：

- 32 个 RV32 通用寄存器，`x0` 恒为 0
- 128 KiB RAM
- `0x30004` 最小停机端口，写入或执行 `ecall` 会停止仿真
- 支持项目测试中使用的 RV32I 指令：算术、逻辑、移位、比较、分支、跳转、Load/Store、LUI/AUIPC
- 额外支持当前 Verilog 解码中出现的 `MUL`、`DIV` 行为级执行

`ooo_sim` 额外保留 CPU 内部机制：

- `RegStat` 等价机制：每个寄存器带 `{value, tag}`
- ALU/LS/Branch 保留站
- 两个 ALU 执行单元
- 写回广播，等待 tag 的操作数自动唤醒
- WAW tag 匹配保护
- 控制流指令暂停取指直到重定向完成
- 简单 2-way 指令 cache，统计 hit/miss

`fakecpu.v` 是 Verilog 工程里的演示替身，不属于最终 CPU 功能需求。C++ 主项目不依赖它。

## 构建

```bash
make
```

## 运行内置测试

```bash
./riscv_sim
./ooo_sim
./run_tests.sh
```

清理编译产物：

```bash
make clean
```

## 可视化乱序执行日志

```bash
./ooo_sim tests/ooo_parallel.hex --trace --max-cycles 20
```

日志会输出：

- 每周期 PC、保留站大小、执行单元状态
- 指令分配到哪个保留站
- 源操作数等待哪个 tag
- 指令进入哪个执行单元
- 写回广播 tag 和寄存器
- 分支/跳转重定向
- ecall 等待乱序队列排空后停机

内置程序执行：

```asm
addi x1,x0,10
addi x2,x0,20
add  x3,x1,x2
sw   x3,0(x0)
lw   x3,0(x0)
ecall
```

## 加载十六进制程序

```bash
./riscv_sim program.hex --max-cycles 100000
```

`program.hex` 支持两类 token：

- 8 位字节：每个 token 长度不超过 2，例如 `93 00 a0 00`
- 32 位指令字：例如 `00a00093`

32 位指令按小端写入内存，和 RV32 指令内存布局一致。

## 测试

```bash
./run_tests.sh
```

测试覆盖指令执行、Load/Store、Branch、Jump、`MUL/DIV` 和停机端口。
