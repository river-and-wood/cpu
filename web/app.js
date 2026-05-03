const PRESETS = {
  "tomasulo_rv32: 本项目指令改写": `{"event":"cycle","cycle":0,"pc":0,"rob":0,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"allocate","cycle":0,"pc":0,"inst":41943315,"tag":1,"rd":2,"rs1":0,"rs2":8,"src1_ready":true,"src1_tag":0,"src2_ready":true,"src2_tag":0,"unit":"ALU","predicted_pc":4}
{"event":"cycle","cycle":1,"pc":4,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":1,"fu":"ALU0","pc":0,"inst":41943315,"tag":1,"latency":1}
{"event":"allocate","cycle":1,"pc":4,"inst":41943443,"tag":2,"rd":3,"rs1":0,"rs2":8,"src1_ready":true,"src1_tag":0,"src2_ready":true,"src2_tag":0,"unit":"ALU","predicted_pc":8}
{"event":"cycle","cycle":2,"pc":8,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":2,"tag":1,"rd":2,"value":40}
{"event":"commit","cycle":2,"tag":1,"pc":0,"rd":2,"value":40}
{"event":"issue","cycle":2,"fu":"ALU0","pc":4,"inst":41943443,"tag":2,"latency":1}
{"event":"allocate","cycle":2,"pc":8,"inst":4196115,"tag":3,"rd":14,"rs1":0,"rs2":4,"src1_ready":true,"src1_tag":0,"src2_ready":true,"src2_tag":0,"unit":"ALU","predicted_pc":12}
{"event":"cycle","cycle":3,"pc":12,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":3,"tag":2,"rd":3,"value":40}
{"event":"commit","cycle":3,"tag":2,"pc":4,"rd":3,"value":40}
{"event":"issue","cycle":3,"fu":"ALU0","pc":8,"inst":4196115,"tag":3,"latency":1}
{"event":"allocate","cycle":3,"pc":12,"inst":74499,"tag":4,"rd":6,"rs1":2,"rs2":0,"src1_ready":true,"src1_tag":0,"src2_ready":true,"src2_tag":0,"unit":"LS","predicted_pc":16}
{"event":"cycle","cycle":4,"pc":16,"rob":2,"alu_rs":0,"ls_rs":1,"branch_rs":0}
{"event":"writeback","cycle":4,"tag":3,"rd":14,"value":4}
{"event":"commit","cycle":4,"tag":3,"pc":8,"rd":14,"value":4}
{"event":"issue","cycle":4,"fu":"LS","pc":12,"inst":74499,"tag":4,"latency":2}
{"event":"allocate","cycle":4,"pc":16,"inst":4302339,"tag":5,"rd":12,"rs1":3,"rs2":4,"src1_ready":true,"src1_tag":0,"src2_ready":true,"src2_tag":0,"unit":"LS","predicted_pc":20}
{"event":"cycle","cycle":5,"pc":20,"rob":2,"alu_rs":0,"ls_rs":1,"branch_rs":0}
{"event":"allocate","cycle":5,"pc":20,"inst":48629043,"tag":6,"rd":10,"rs1":12,"rs2":14,"src1_ready":false,"src1_tag":5,"src2_ready":true,"src2_tag":0,"unit":"ALU","predicted_pc":24}
{"event":"cycle","cycle":6,"pc":24,"rob":3,"alu_rs":1,"ls_rs":1,"branch_rs":0}
{"event":"writeback","cycle":6,"tag":4,"rd":6,"value":9}
{"event":"commit","cycle":6,"tag":4,"pc":12,"rd":6,"value":9}
{"event":"issue","cycle":6,"fu":"LS","pc":16,"inst":4302339,"tag":5,"latency":2}
{"event":"allocate","cycle":6,"pc":24,"inst":1086523699,"tag":7,"rd":18,"rs1":6,"rs2":12,"src1_ready":true,"src1_tag":0,"src2_ready":false,"src2_tag":5,"unit":"ALU","predicted_pc":28}
{"event":"cycle","cycle":7,"pc":28,"rob":3,"alu_rs":2,"ls_rs":0,"branch_rs":0}
{"event":"allocate","cycle":7,"pc":28,"inst":40192563,"tag":8,"rd":20,"rs1":10,"rs2":6,"src1_ready":false,"src1_tag":6,"src2_ready":true,"src2_tag":0,"unit":"ALU","predicted_pc":32}
{"event":"cycle","cycle":8,"pc":32,"rob":4,"alu_rs":3,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":8,"tag":5,"rd":12,"value":3}
{"event":"commit","cycle":8,"tag":5,"pc":16,"rd":12,"value":3}
{"event":"issue","cycle":8,"fu":"ALU0","pc":20,"inst":48629043,"tag":6,"latency":3}
{"event":"issue","cycle":8,"fu":"ALU1","pc":24,"inst":1086523699,"tag":7,"latency":1}
{"event":"allocate","cycle":8,"pc":32,"inst":13173555,"tag":9,"rd":6,"rs1":18,"rs2":12,"src1_ready":false,"src1_tag":7,"src2_ready":true,"src2_tag":0,"unit":"ALU","predicted_pc":36}
{"event":"cycle","cycle":9,"pc":36,"rob":4,"alu_rs":2,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":9,"tag":7,"rd":18,"value":6}
{"event":"issue","cycle":9,"fu":"ALU1","pc":32,"inst":13173555,"tag":9,"latency":1}
{"event":"allocate","cycle":9,"pc":36,"inst":115,"tag":10,"kind":"ecall"}
{"event":"cycle","cycle":10,"pc":40,"rob":5,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":10,"tag":9,"rd":6,"value":9}
{"event":"stall","cycle":10,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":11,"pc":40,"rob":5,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":11,"tag":6,"rd":10,"value":12}
{"event":"commit","cycle":11,"tag":6,"pc":20,"rd":10,"value":12}
{"event":"issue","cycle":11,"fu":"ALU0","pc":28,"inst":40192563,"tag":8,"latency":35}
{"event":"stall","cycle":11,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":12,"pc":40,"rob":4,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":12,"tag":7,"pc":24,"rd":18,"value":6}
{"event":"stall","cycle":12,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":13,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":13,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":14,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":14,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":15,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":15,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":16,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":16,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":17,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":17,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":18,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":18,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":19,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":19,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":20,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":20,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":21,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":21,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":22,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":22,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":23,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":23,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":24,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":24,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":25,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":25,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":26,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":26,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":27,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":27,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":28,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":28,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":29,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":29,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":30,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":30,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":31,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":31,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":32,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":32,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":33,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":33,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":34,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":34,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":35,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":35,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":36,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":36,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":37,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":37,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":38,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":38,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":39,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":39,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":40,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":40,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":41,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":41,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":42,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":42,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":43,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":43,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":44,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":44,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":45,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":45,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":46,"pc":40,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":46,"tag":8,"rd":20,"value":1}
{"event":"commit","cycle":46,"tag":8,"pc":28,"rd":20,"value":1}
{"event":"stall","cycle":46,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":47,"pc":40,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":47,"tag":9,"pc":32,"rd":6,"value":9}
{"event":"stall","cycle":47,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":48,"pc":40,"rob":1,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":48,"tag":10,"pc":36,"ecall":true}
{"stats":{"cycles":49,"committed_insts":10,"ipc":0.204082,"branch_predictions":0,"branch_mispredictions":0,"flushes":0,"rs_stalls":0,"rob_stalls":0,"store_commits":0,"icache_hits":0,"icache_misses":10,"icache_hit_rate":0}}`,
  "教学假设: 原始浮点序列": `{"event":"cycle","cycle":0,"pc":0,"rob":0,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"note","cycle":0,"reason":"教学假设：LD=2 cycles, MULT=4 cycles, SUBD/ADDD=2 cycles, DIVD=8 cycles；每条指令进入 ROB 并按序提交。"}
{"event":"allocate","cycle":0,"pc":0,"tag":1,"unit":"LS","asm":"LD F6, 34(R2)","predicted_pc":4}
{"event":"cycle","cycle":1,"pc":4,"rob":1,"alu_rs":0,"ls_rs":1,"branch_rs":0}
{"event":"issue","cycle":1,"fu":"LS","pc":0,"tag":1,"latency":2,"asm":"LD F6, 34(R2)"}
{"event":"allocate","cycle":1,"pc":4,"tag":2,"unit":"LS","asm":"LD F2, 45(R3)","predicted_pc":8}
{"event":"cycle","cycle":2,"pc":8,"rob":2,"alu_rs":0,"ls_rs":1,"branch_rs":0}
{"event":"allocate","cycle":2,"pc":8,"tag":3,"unit":"MUL","asm":"MULT F0, F2, F4","predicted_pc":12}
{"event":"cycle","cycle":3,"pc":12,"rob":3,"alu_rs":1,"ls_rs":1,"branch_rs":0}
{"event":"writeback","cycle":3,"tag":1,"rd":"F6","value":"Mem[R2+34]"}
{"event":"commit","cycle":3,"tag":1,"pc":0,"rd":"F6","value":"Mem[R2+34]"}
{"event":"issue","cycle":3,"fu":"LS","pc":4,"tag":2,"latency":2,"asm":"LD F2, 45(R3)"}
{"event":"allocate","cycle":3,"pc":12,"tag":4,"unit":"ADD","asm":"SUBD F8, F6, F2","predicted_pc":16}
{"event":"cycle","cycle":4,"pc":16,"rob":3,"alu_rs":2,"ls_rs":0,"branch_rs":0}
{"event":"allocate","cycle":4,"pc":16,"tag":5,"unit":"DIV","asm":"DIVD F10, F0, F6","predicted_pc":20}
{"event":"cycle","cycle":5,"pc":20,"rob":4,"alu_rs":3,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":5,"tag":2,"rd":"F2","value":"Mem[R3+45]"}
{"event":"commit","cycle":5,"tag":2,"pc":4,"rd":"F2","value":"Mem[R3+45]"}
{"event":"issue","cycle":5,"fu":"MUL","pc":8,"tag":3,"latency":4,"asm":"MULT F0, F2, F4"}
{"event":"issue","cycle":5,"fu":"ADD","pc":12,"tag":4,"latency":2,"asm":"SUBD F8, F6, F2"}
{"event":"allocate","cycle":5,"pc":20,"tag":6,"unit":"ADD","asm":"ADDD F6, F8, F2","predicted_pc":24}
{"event":"cycle","cycle":6,"pc":24,"rob":4,"alu_rs":2,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":6,"reason":"示例程序已全部分配，等待执行完成"}
{"event":"cycle","cycle":7,"pc":24,"rob":4,"alu_rs":2,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":7,"tag":4,"rd":"F8","value":"F6-F2"}
{"event":"stall","cycle":7,"reason":"T6 等待 T4 的 F8；T5 等待 T3 的 F0"}
{"event":"cycle","cycle":8,"pc":24,"rob":4,"alu_rs":2,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":8,"fu":"ADD","pc":20,"tag":6,"latency":2,"asm":"ADDD F6, F8, F2"}
{"event":"stall","cycle":8,"reason":"ROB head T3 尚未完成，T4 已完成但不能越过提交"}
{"event":"cycle","cycle":9,"pc":24,"rob":4,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":9,"tag":3,"rd":"F0","value":"F2*F4"}
{"event":"commit","cycle":9,"tag":3,"pc":8,"rd":"F0","value":"F2*F4"}
{"event":"issue","cycle":9,"fu":"DIV","pc":16,"tag":5,"latency":8,"asm":"DIVD F10, F0, F6"}
{"event":"cycle","cycle":10,"pc":24,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":10,"tag":6,"rd":"F6","value":"F8+F2"}
{"event":"commit","cycle":10,"tag":4,"pc":12,"rd":"F8","value":"F6-F2"}
{"event":"cycle","cycle":11,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":11,"reason":"T6 已完成但等待更老的 T5 DIVD 提交；体现 ROB 按序提交"}
{"event":"cycle","cycle":12,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":12,"reason":"DIVD 长延迟执行中"}
{"event":"cycle","cycle":13,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":13,"reason":"DIVD 长延迟执行中"}
{"event":"cycle","cycle":14,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":14,"reason":"DIVD 长延迟执行中"}
{"event":"cycle","cycle":15,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":15,"reason":"DIVD 长延迟执行中"}
{"event":"cycle","cycle":16,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"stall","cycle":16,"reason":"DIVD 长延迟执行中"}
{"event":"cycle","cycle":17,"pc":24,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":17,"tag":5,"rd":"F10","value":"F0/F6"}
{"event":"commit","cycle":17,"tag":5,"pc":16,"rd":"F10","value":"F0/F6"}
{"event":"cycle","cycle":18,"pc":24,"rob":1,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":18,"tag":6,"pc":20,"rd":"F6","value":"F8+F2"}
{"stats":{"cycles":19,"committed_insts":6,"ipc":0.315789,"branch_predictions":0,"branch_mispredictions":0,"flushes":0,"rs_stalls":0,"rob_stalls":0,"store_commits":0,"icache_hits":0,"icache_misses":0,"icache_hit_rate":0}}`,
  "ooo_parallel: ALU 并行与长延迟 MUL": `{"event":"cycle","cycle":0,"pc":0,"rob":0,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"allocate","cycle":0,"pc":0,"inst":6291603,"tag":1,"unit":"ALU","predicted_pc":4}
{"event":"cycle","cycle":1,"pc":4,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":1,"fu":"ALU0","pc":0,"inst":6291603,"tag":1,"latency":1}
{"event":"allocate","cycle":1,"pc":4,"inst":7340307,"tag":2,"unit":"ALU","predicted_pc":8}
{"event":"cycle","cycle":2,"pc":8,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":2,"tag":1,"rd":1,"value":6}
{"event":"commit","cycle":2,"tag":1,"pc":0,"rd":1,"value":6}
{"event":"issue","cycle":2,"fu":"ALU0","pc":4,"inst":7340307,"tag":2,"latency":1}
{"event":"allocate","cycle":2,"pc":8,"inst":1049235,"tag":3,"unit":"ALU","predicted_pc":12}
{"event":"cycle","cycle":3,"pc":12,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":3,"tag":2,"rd":2,"value":7}
{"event":"commit","cycle":3,"tag":2,"pc":4,"rd":2,"value":7}
{"event":"issue","cycle":3,"fu":"ALU0","pc":8,"inst":1049235,"tag":3,"latency":1}
{"event":"allocate","cycle":3,"pc":12,"inst":2097939,"tag":4,"unit":"ALU","predicted_pc":16}
{"event":"cycle","cycle":4,"pc":16,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":4,"tag":3,"rd":5,"value":1}
{"event":"commit","cycle":4,"tag":3,"pc":8,"rd":5,"value":1}
{"event":"issue","cycle":4,"fu":"ALU0","pc":12,"inst":2097939,"tag":4,"latency":1}
{"event":"allocate","cycle":4,"pc":16,"inst":35684787,"tag":5,"unit":"ALU","predicted_pc":20}
{"event":"cycle","cycle":5,"pc":20,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":5,"tag":4,"rd":6,"value":2}
{"event":"commit","cycle":5,"tag":4,"pc":12,"rd":6,"value":2}
{"event":"issue","cycle":5,"fu":"ALU0","pc":16,"inst":35684787,"tag":5,"latency":3}
{"event":"allocate","cycle":5,"pc":20,"inst":6455859,"tag":6,"unit":"ALU","predicted_pc":24}
{"event":"cycle","cycle":6,"pc":24,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":6,"fu":"ALU1","pc":20,"inst":6455859,"tag":6,"latency":1}
{"event":"allocate","cycle":6,"pc":24,"inst":4293555,"tag":7,"unit":"ALU","predicted_pc":28}
{"event":"cycle","cycle":7,"pc":28,"rob":3,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":7,"tag":6,"rd":4,"value":3}
{"event":"allocate","cycle":7,"pc":28,"inst":115,"tag":8,"kind":"ecall"}
{"event":"cycle","cycle":8,"pc":32,"rob":4,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":8,"tag":5,"rd":3,"value":42}
{"event":"commit","cycle":8,"tag":5,"pc":16,"rd":3,"value":42}
{"event":"issue","cycle":8,"fu":"ALU0","pc":24,"inst":4293555,"tag":7,"latency":1}
{"event":"stall","cycle":8,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":9,"pc":32,"rob":3,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":9,"tag":7,"rd":7,"value":45}
{"event":"commit","cycle":9,"tag":6,"pc":20,"rd":4,"value":3}
{"event":"stall","cycle":9,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":10,"pc":32,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":10,"tag":7,"pc":24,"rd":7,"value":45}
{"event":"stall","cycle":10,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":11,"pc":32,"rob":1,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":11,"tag":8,"pc":28,"ecall":true}
{"stats":{"cycles":12,"committed_insts":8,"ipc":0.666667,"branch_predictions":0,"branch_mispredictions":0,"flushes":0,"rs_stalls":0,"rob_stalls":0,"store_commits":0,"icache_hits":0,"icache_misses":8,"icache_hit_rate":0}}`,
  "branch_all: 分支预测失败与 Flush": `{"event":"cycle","cycle":0,"pc":0,"rob":0,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"allocate","cycle":0,"pc":0,"inst":5243027,"tag":1,"unit":"ALU","predicted_pc":4}
{"event":"cycle","cycle":1,"pc":4,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":1,"fu":"ALU0","pc":0,"inst":5243027,"tag":1,"latency":1}
{"event":"allocate","cycle":1,"pc":4,"inst":5243155,"tag":2,"unit":"ALU","predicted_pc":8}
{"event":"cycle","cycle":2,"pc":8,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":2,"tag":1,"rd":1,"value":5}
{"event":"commit","cycle":2,"tag":1,"pc":0,"rd":1,"value":5}
{"event":"issue","cycle":2,"fu":"ALU0","pc":4,"inst":5243155,"tag":2,"latency":1}
{"event":"allocate","cycle":2,"pc":8,"inst":3146131,"tag":3,"unit":"ALU","predicted_pc":12}
{"event":"cycle","cycle":3,"pc":12,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":3,"tag":2,"rd":2,"value":5}
{"event":"commit","cycle":3,"tag":2,"pc":4,"rd":2,"value":5}
{"event":"issue","cycle":3,"fu":"ALU0","pc":8,"inst":3146131,"tag":3,"latency":1}
{"event":"allocate","cycle":3,"pc":12,"inst":2131043,"tag":4,"unit":"BR","predicted_pc":16}
{"event":"cycle","cycle":4,"pc":16,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":1}
{"event":"writeback","cycle":4,"tag":3,"rd":3,"value":3}
{"event":"commit","cycle":4,"tag":3,"pc":8,"rd":3,"value":3}
{"event":"issue","cycle":4,"fu":"BR","pc":12,"inst":2131043,"tag":4,"latency":1}
{"event":"allocate","cycle":4,"pc":16,"inst":1049875,"tag":5,"unit":"ALU","predicted_pc":20}
{"event":"cycle","cycle":5,"pc":20,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":5,"tag":4}
{"event":"redirect","cycle":5,"pc":20}
{"event":"flush","cycle":5,"after_tag":4,"pc":20}
{"event":"commit","cycle":5,"tag":4,"pc":12}
{"event":"allocate","cycle":5,"pc":20,"inst":9438483,"tag":6,"unit":"ALU","predicted_pc":24}
{"event":"cycle","cycle":6,"pc":24,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":6,"fu":"ALU0","pc":20,"inst":9438483,"tag":6,"latency":1}
{"event":"allocate","cycle":6,"pc":24,"inst":3183715,"tag":7,"unit":"BR","predicted_pc":28}
{"event":"cycle","cycle":7,"pc":28,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":1}
{"event":"writeback","cycle":7,"tag":6,"rd":10,"value":9}
{"event":"commit","cycle":7,"tag":6,"pc":20,"rd":10,"value":9}
{"event":"issue","cycle":7,"fu":"BR","pc":24,"inst":3183715,"tag":7,"latency":1}
{"event":"allocate","cycle":7,"pc":28,"inst":1050003,"tag":8,"unit":"ALU","predicted_pc":32}
{"event":"cycle","cycle":8,"pc":32,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":8,"tag":7}
{"event":"redirect","cycle":8,"pc":32}
{"event":"flush","cycle":8,"after_tag":7,"pc":32}
{"event":"commit","cycle":8,"tag":7,"pc":24}
{"event":"allocate","cycle":8,"pc":32,"inst":9438611,"tag":9,"unit":"ALU","predicted_pc":36}
{"event":"cycle","cycle":9,"pc":36,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":9,"fu":"ALU0","pc":32,"inst":9438611,"tag":9,"latency":1}
{"event":"allocate","cycle":9,"pc":36,"inst":1164387,"tag":10,"unit":"BR","predicted_pc":40}
{"event":"cycle","cycle":10,"pc":40,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":1}
{"event":"writeback","cycle":10,"tag":9,"rd":11,"value":9}
{"event":"commit","cycle":10,"tag":9,"pc":32,"rd":11,"value":9}
{"event":"issue","cycle":10,"fu":"BR","pc":36,"inst":1164387,"tag":10,"latency":1}
{"event":"allocate","cycle":10,"pc":40,"inst":1050131,"tag":11,"unit":"ALU","predicted_pc":44}
{"event":"cycle","cycle":11,"pc":44,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":11,"tag":10}
{"event":"redirect","cycle":11,"pc":44}
{"event":"flush","cycle":11,"after_tag":10,"pc":44}
{"event":"commit","cycle":11,"tag":10,"pc":36}
{"event":"allocate","cycle":11,"pc":44,"inst":9438739,"tag":12,"unit":"ALU","predicted_pc":48}
{"event":"cycle","cycle":12,"pc":48,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":12,"fu":"ALU0","pc":44,"inst":9438739,"tag":12,"latency":1}
{"event":"allocate","cycle":12,"pc":48,"inst":3200099,"tag":13,"unit":"BR","predicted_pc":52}
{"event":"cycle","cycle":13,"pc":52,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":1}
{"event":"writeback","cycle":13,"tag":12,"rd":12,"value":9}
{"event":"commit","cycle":13,"tag":12,"pc":44,"rd":12,"value":9}
{"event":"issue","cycle":13,"fu":"BR","pc":48,"inst":3200099,"tag":13,"latency":1}
{"event":"allocate","cycle":13,"pc":52,"inst":1050259,"tag":14,"unit":"ALU","predicted_pc":56}
{"event":"cycle","cycle":14,"pc":56,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":14,"tag":13}
{"event":"redirect","cycle":14,"pc":56}
{"event":"flush","cycle":14,"after_tag":13,"pc":56}
{"event":"commit","cycle":14,"tag":13,"pc":48}
{"event":"allocate","cycle":14,"pc":56,"inst":9438867,"tag":15,"unit":"ALU","predicted_pc":60}
{"event":"cycle","cycle":15,"pc":60,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":15,"fu":"ALU0","pc":56,"inst":9438867,"tag":15,"latency":1}
{"event":"allocate","cycle":15,"pc":60,"inst":1172579,"tag":16,"unit":"BR","predicted_pc":64}
{"event":"cycle","cycle":16,"pc":64,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":1}
{"event":"writeback","cycle":16,"tag":15,"rd":13,"value":9}
{"event":"commit","cycle":16,"tag":15,"pc":56,"rd":13,"value":9}
{"event":"issue","cycle":16,"fu":"BR","pc":60,"inst":1172579,"tag":16,"latency":1}
{"event":"allocate","cycle":16,"pc":64,"inst":1050387,"tag":17,"unit":"ALU","predicted_pc":68}
{"event":"cycle","cycle":17,"pc":68,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":17,"tag":16}
{"event":"redirect","cycle":17,"pc":68}
{"event":"flush","cycle":17,"after_tag":16,"pc":68}
{"event":"commit","cycle":17,"tag":16,"pc":60}
{"event":"allocate","cycle":17,"pc":68,"inst":9438995,"tag":18,"unit":"ALU","predicted_pc":72}
{"event":"cycle","cycle":18,"pc":72,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":18,"fu":"ALU0","pc":68,"inst":9438995,"tag":18,"latency":1}
{"event":"allocate","cycle":18,"pc":72,"inst":3208291,"tag":19,"unit":"BR","predicted_pc":76}
{"event":"cycle","cycle":19,"pc":76,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":1}
{"event":"writeback","cycle":19,"tag":18,"rd":14,"value":9}
{"event":"commit","cycle":19,"tag":18,"pc":68,"rd":14,"value":9}
{"event":"issue","cycle":19,"fu":"BR","pc":72,"inst":3208291,"tag":19,"latency":1}
{"event":"allocate","cycle":19,"pc":76,"inst":1050515,"tag":20,"unit":"ALU","predicted_pc":80}
{"event":"cycle","cycle":20,"pc":80,"rob":2,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":20,"tag":19}
{"event":"redirect","cycle":20,"pc":80}
{"event":"flush","cycle":20,"after_tag":19,"pc":80}
{"event":"commit","cycle":20,"tag":19,"pc":72}
{"event":"allocate","cycle":20,"pc":80,"inst":9439123,"tag":21,"unit":"ALU","predicted_pc":84}
{"event":"cycle","cycle":21,"pc":84,"rob":1,"alu_rs":1,"ls_rs":0,"branch_rs":0}
{"event":"issue","cycle":21,"fu":"ALU0","pc":80,"inst":9439123,"tag":21,"latency":1}
{"event":"allocate","cycle":21,"pc":84,"inst":115,"tag":22,"kind":"ecall"}
{"event":"cycle","cycle":22,"pc":88,"rob":2,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"writeback","cycle":22,"tag":21,"rd":15,"value":9}
{"event":"commit","cycle":22,"tag":21,"pc":80,"rd":15,"value":9}
{"event":"stall","cycle":22,"reason":"控制指令未决，暂停取指"}
{"event":"cycle","cycle":23,"pc":88,"rob":1,"alu_rs":0,"ls_rs":0,"branch_rs":0}
{"event":"commit","cycle":23,"tag":22,"pc":84,"ecall":true}
{"stats":{"cycles":24,"committed_insts":16,"ipc":0.666667,"branch_predictions":6,"branch_mispredictions":6,"flushes":6,"rs_stalls":0,"rob_stalls":0,"store_commits":0,"icache_hits":0,"icache_misses":22,"icache_hit_rate":0}}`
};

const els = {
  presetSelect: document.getElementById("presetSelect"),
  loadPresetBtn: document.getElementById("loadPresetBtn"),
  prevBtn: document.getElementById("prevBtn"),
  playBtn: document.getElementById("playBtn"),
  nextBtn: document.getElementById("nextBtn"),
  cycleSlider: document.getElementById("cycleSlider"),
  cycleLabel: document.getElementById("cycleLabel"),
  traceFile: document.getElementById("traceFile"),
  pasteLoadBtn: document.getElementById("pasteLoadBtn"),
  clearPasteBtn: document.getElementById("clearPasteBtn"),
  traceInput: document.getElementById("traceInput"),
  statsGrid: document.getElementById("statsGrid"),
  storyTitle: document.getElementById("storyTitle"),
  storyText: document.getElementById("storyText"),
  storyFacts: document.getElementById("storyFacts"),
  currentSummary: document.getElementById("currentSummary"),
  cycleRail: document.getElementById("cycleRail"),
  componentFlow: document.getElementById("componentFlow"),
  programTable: document.getElementById("programTable"),
  fetchStage: document.getElementById("fetchStage"),
  rsStage: document.getElementById("rsStage"),
  execStage: document.getElementById("execStage"),
  wbStage: document.getElementById("wbStage"),
  commitStage: document.getElementById("commitStage"),
  redirectStage: document.getElementById("redirectStage"),
  timeline: document.getElementById("timeline"),
  eventList: document.getElementById("eventList"),
  diagram: document.getElementById("diagram")
};

let model = null;
let currentCycle = 0;
let playTimer = null;

function hex(n, width = 0) {
  const value = Number(n >>> 0).toString(16);
  return "0x" + value.padStart(width, "0");
}

function signExtend(value, bits) {
  const shift = 32 - bits;
  return (value << shift) >> shift;
}

function bits(x, lo, width) {
  return (x >>> lo) & ((1 << width) - 1);
}

function regName(n) {
  if (typeof n === "string") return n;
  return "x" + Number(n);
}

function valueText(value) {
  if (value == null) return "";
  return typeof value === "number" ? hex(value) : String(value);
}

function disassemble(raw) {
  if (raw == null) return "custom instruction";
  const inst = Number(raw) >>> 0;
  if (inst === 0x00000073) return "ecall";
  const opcode = bits(inst, 0, 7);
  const rd = bits(inst, 7, 5);
  const funct3 = bits(inst, 12, 3);
  const rs1 = bits(inst, 15, 5);
  const rs2 = bits(inst, 20, 5);
  const funct7 = bits(inst, 25, 7);
  const iImm = signExtend(inst >>> 20, 12);
  const sImm = signExtend(bits(inst, 7, 5) | (bits(inst, 25, 7) << 5), 12);
  const bImm = signExtend((bits(inst, 8, 4) << 1) | (bits(inst, 25, 6) << 5) | (bits(inst, 7, 1) << 11) | (bits(inst, 31, 1) << 12), 13);
  const uImm = inst & 0xfffff000;
  const jImm = signExtend((bits(inst, 21, 10) << 1) | (bits(inst, 20, 1) << 11) | (bits(inst, 12, 8) << 12) | (bits(inst, 31, 1) << 20), 21);
  const branch = ["beq", "bne", "?", "?", "blt", "bge", "bltu", "bgeu"];
  const load = ["lb", "lh", "lw", "?", "lbu", "lhu"];
  const store = ["sb", "sh", "sw"];
  const opImm = ["addi", "slli", "slti", "sltiu", "xori", "srli", "ori", "andi"];
  const op = ["add", "sll", "slt", "sltu", "xor", "srl", "or", "and"];
  if (opcode === 0x37) return `lui ${regName(rd)}, ${hex(uImm)}`;
  if (opcode === 0x17) return `auipc ${regName(rd)}, ${hex(uImm)}`;
  if (opcode === 0x6f) return `jal ${regName(rd)}, ${jImm}`;
  if (opcode === 0x67) return `jalr ${regName(rd)}, ${iImm}(${regName(rs1)})`;
  if (opcode === 0x63) return `${branch[funct3] || "branch"} ${regName(rs1)}, ${regName(rs2)}, ${bImm}`;
  if (opcode === 0x03) return `${load[funct3] || "load"} ${regName(rd)}, ${iImm}(${regName(rs1)})`;
  if (opcode === 0x23) return `${store[funct3] || "store"} ${regName(rs2)}, ${sImm}(${regName(rs1)})`;
  if (opcode === 0x13) {
    if (funct3 === 5 && funct7 === 0x20) return `srai ${regName(rd)}, ${regName(rs1)}, ${bits(inst, 20, 5)}`;
    if (funct3 === 1 || funct3 === 5) return `${opImm[funct3]} ${regName(rd)}, ${regName(rs1)}, ${bits(inst, 20, 5)}`;
    return `${opImm[funct3] || "opimm"} ${regName(rd)}, ${regName(rs1)}, ${iImm}`;
  }
  if (opcode === 0x33) {
    let name = op[funct3] || "op";
    if (funct3 === 0 && funct7 === 0x20) name = "sub";
    if (funct7 === 0x01 && funct3 === 0) name = "mul";
    if (funct7 === 0x01 && funct3 === 4) name = "div";
    return `${name} ${regName(rd)}, ${regName(rs1)}, ${regName(rs2)}`;
  }
  return hex(inst, 8);
}

function instLabel(ev) {
  if (ev.asm) return ev.asm;
  if (ev.kind === "ecall") return "ecall";
  return ev.inst != null ? disassemble(ev.inst) : "custom instruction";
}

function teachingHint(raw) {
  const hints = {
    41943315: "初始化 x2，指向数据区",
    41943443: "初始化 x3，指向数据区",
    4196115: "初始化 x14=4，对应原示例的 F4",
    74499: "对应 LD F6：从内存读入 x6",
    4302339: "对应 LD F2：从内存读入 x12",
    48629043: "对应 MULT F0,F2,F4：x10=x12*x14",
    1086523699: "对应 SUBD F8,F6,F2：x18=x6-x12",
    40192563: "对应 DIVD F10,F0,F6：x20=x10/x6，长延迟",
    13173555: "对应 ADDD F6,F8,F2：x6=x18+x12，展示 WAW/重命名",
    115: "ecall，等待前面指令按序提交后停机"
  };
  return hints[Number(raw)] || "";
}

function parseTrace(text) {
  const parsed = [];
  for (const line of text.split(/\r?\n/)) {
    const trimmed = line.trim();
    if (!trimmed || trimmed[0] !== "{") continue;
    parsed.push(JSON.parse(trimmed));
  }
  const events = parsed.filter((item) => !item.stats);
  const stats = parsed.find((item) => item.stats)?.stats || {};
  const byCycle = new Map();
  const cycleMeta = new Map();
  const insts = new Map();

  for (const ev of events) {
    if (!byCycle.has(ev.cycle)) byCycle.set(ev.cycle, []);
    byCycle.get(ev.cycle).push(ev);
    if (ev.event === "cycle") cycleMeta.set(ev.cycle, ev);
    if (ev.event === "allocate") {
      insts.set(ev.tag, {
        tag: ev.tag,
        pc: ev.pc,
        raw: ev.inst,
        unit: ev.unit || ev.kind || "?",
        allocCycle: ev.cycle,
        predictedPc: ev.predicted_pc,
        rd: ev.rd,
        rs1: ev.rs1,
        rs2: ev.rs2,
        src1Ready: ev.src1_ready,
        src1Tag: ev.src1_tag,
        src2Ready: ev.src2_ready,
        src2Tag: ev.src2_tag,
        hint: teachingHint(ev.inst),
        label: instLabel(ev)
      });
    }
    if (ev.event === "issue") {
      const item = ensureInst(insts, ev);
      item.issueCycle = ev.cycle;
      item.fu = ev.fu;
      item.latency = ev.latency;
      item.raw = item.raw || ev.inst;
      item.label = item.label || instLabel(ev);
    }
    if (ev.event === "writeback") {
      const item = ensureInst(insts, ev);
      item.wbCycle = ev.cycle;
      item.rd = ev.rd;
      item.value = ev.value;
      item.storeAddr = ev.store_addr;
      item.storeValue = ev.store_value;
    }
    if (ev.event === "commit") {
      const item = ensureInst(insts, ev);
      item.commitCycle = ev.cycle;
      item.pc = item.pc ?? ev.pc;
      item.rd = ev.rd ?? item.rd;
      item.value = ev.value ?? item.value;
      if (ev.ecall) item.label = "ecall";
    }
    if (ev.event === "flush") {
      for (const item of insts.values()) {
        if (item.tag > ev.after_tag && item.allocCycle <= ev.cycle && item.commitCycle == null) {
          item.flushCycle = ev.cycle;
        }
      }
    }
  }

  const maxCycle = Math.max(0, ...events.map((ev) => ev.cycle), Number(stats.cycles || 1) - 1);
  return { events, stats, byCycle, cycleMeta, insts: [...insts.values()].sort((a, b) => a.tag - b.tag), maxCycle };
}

function ensureInst(insts, ev) {
  if (!insts.has(ev.tag)) {
    insts.set(ev.tag, { tag: ev.tag, allocCycle: ev.cycle, label: instLabel(ev) });
  }
  return insts.get(ev.tag);
}

function setTrace(text) {
  try {
    model = parseTrace(text);
    currentCycle = 0;
    els.cycleSlider.max = model.maxCycle;
    els.cycleSlider.value = 0;
    renderAll();
  } catch (err) {
    alert("trace 解析失败: " + err.message);
  }
}

function renderAll() {
  renderStats();
  renderCycleRail();
  renderCycle(currentCycle);
  renderProgramTable();
  renderTimeline();
}

function renderStats() {
  const stats = model?.stats || {};
  const items = [
    ["cycles", stats.cycles],
    ["committed", stats.committed_insts],
    ["IPC", stats.ipc != null ? Number(stats.ipc).toFixed(3) : "-"],
    ["branch miss", stats.branch_mispredictions],
    ["flushes", stats.flushes],
    ["I$ hit rate", stats.icache_hit_rate != null ? (Number(stats.icache_hit_rate) * 100).toFixed(1) + "%" : "-"]
  ];
  els.statsGrid.innerHTML = items.map(([name, value]) => `<div class="stat"><span>${name}</span><strong>${value ?? "-"}</strong></div>`).join("");
}

function renderCycleRail() {
  const buttons = [];
  for (let c = 0; c <= model.maxCycle; c++) {
    const events = model.byCycle.get(c) || [];
    const flags = ["allocate", "issue", "writeback", "commit", "flush"].filter((type) => events.some((ev) => ev.event === type));
    buttons.push(`<button class="cycle-btn ${c === currentCycle ? "active" : ""}" data-cycle="${c}" type="button">
      <span>${c}</span>
      <span class="cycle-flags">${flags.map((f) => `<i class="flag ${flagClass(f)}"></i>`).join("")}</span>
    </button>`);
  }
  els.cycleRail.innerHTML = buttons.join("");
  els.cycleRail.querySelectorAll("button").forEach((button) => {
    button.addEventListener("click", () => goToCycle(Number(button.dataset.cycle)));
  });
}

function renderCycle(cycle) {
  const events = model.byCycle.get(cycle) || [];
  const meta = model.cycleMeta.get(cycle);
  els.cycleLabel.textContent = `cycle ${cycle}`;
  els.cycleSlider.value = cycle;
  els.currentSummary.textContent = meta
    ? `PC=${hex(meta.pc)} · ROB=${meta.rob} · ALU_RS=${meta.alu_rs} · LS_RS=${meta.ls_rs} · BR_RS=${meta.branch_rs}`
    : "该周期没有 cycle 快照事件";

  const alloc = events.filter((ev) => ev.event === "allocate");
  const issue = events.filter((ev) => ev.event === "issue");
  const wb = events.filter((ev) => ev.event === "writeback");
  const commit = events.filter((ev) => ev.event === "commit");
  const redirect = events.filter((ev) => ev.event === "redirect" || ev.event === "flush");
  const stall = events.filter((ev) => ev.event === "stall");
  const waitingItems = model.insts.filter((item) => item.label !== "ecall" && item.allocCycle <= cycle && item.issueCycle == null && operandWaitText(item, cycle));
  const readyItems = model.insts.filter((item) => item.label !== "ecall" && item.allocCycle <= cycle && item.issueCycle == null && !operandWaitText(item, cycle));

  renderStage(els.fetchStage, [
    meta ? chip("alloc", "取指 PC", hex(meta.pc), `ROB ${meta.rob}`) : "",
    ...alloc.map((ev) => chip("alloc", `T${ev.tag} 分配到 ${ev.unit || ev.kind}`, instLabel(ev), `pc=${hex(ev.pc)} -> predicted=${hex(ev.predicted_pc ?? ev.pc + 4)}`)),
    ...stall.map((ev) => chip("stall", "暂停取指", ev.reason, "PC 保持等待"))
  ]);
  renderStage(els.rsStage, [
    meta ? occupancy("ROB", meta.rob, 16) : "",
    meta ? occupancy("ALU_RS", meta.alu_rs, 8) : "",
    meta ? occupancy("LS_RS", meta.ls_rs, 8) : "",
    meta ? occupancy("BR_RS", meta.branch_rs, 4) : "",
    ...waitingItems.map((item) => chip("stall", `T${item.tag} 等待操作数`, item.label, operandWaitText(item, cycle))),
    ...readyItems.map((item) => chip("alloc", `T${item.tag} 已就绪`, item.label, "等待空闲执行单元"))
  ]);
  renderStage(els.execStage, [
    ...issue.map((ev) => chip("issue", `${ev.fu} 开始执行 T${ev.tag}`, instLabel(ev), `latency=${ev.latency}`)),
    ...activeExecutions(cycle).map((item) => chip("issue", `${item.fu} 执行中 T${item.tag}`, item.label, `issue=${item.issueCycle}, wb=${item.wbCycle ?? "?"}`))
  ]);
  renderStage(els.wbStage, wb.map((ev) => chip("wb", `T${ev.tag} 写回`, writebackText(ev), "广播唤醒等待该 tag 的操作数")));
  renderStage(els.commitStage, commit.map((ev) => chip("commit", `T${ev.tag} 按序提交`, commitText(ev), ev.ecall ? "停机端口触发" : "更新架构状态")));
  renderStage(els.redirectStage, [
    ...redirect.map((ev) => ev.event === "redirect"
    ? chip("redirect", "控制重定向", `next_pc=${hex(ev.pc)}`, "恢复正确取指路径")
    : chip("flush", "Flush 年轻指令", `清除 T${ev.after_tag} 之后`, `pc=${hex(ev.pc)}`)),
    ...stall.map((ev) => chip("stall", "等待原因", ev.reason, "没有继续取指"))
  ]);

  renderEvents(events);
  renderStory(cycle, events, meta);
  renderComponentFlow(cycle, events, meta);
  renderProgramTable();
  renderDiagram(events);
  updateCycleRailActive();
}

function renderStage(target, chips) {
  const html = chips.filter(Boolean).join("");
  target.innerHTML = html || `<div class="empty">本周期无事件</div>`;
  target.closest(".stage").classList.toggle("active", Boolean(html));
}

function renderStory(cycle, events, meta) {
  const alloc = events.filter((ev) => ev.event === "allocate");
  const issue = events.filter((ev) => ev.event === "issue");
  const wb = events.filter((ev) => ev.event === "writeback");
  const commit = events.filter((ev) => ev.event === "commit");
  const stall = events.filter((ev) => ev.event === "stall");
  const active = activeExecutions(cycle);

  let title = `Cycle ${cycle}`;
  let text = "这个周期没有可见的流水动作。";
  if (wb.length || commit.length) {
    title = "结果先写回，再由 ROB 按序提交";
    const wbText = wb.map((ev) => `T${ev.tag} 写回 ${writebackText(ev)}`).join("；");
    const commitTextLine = commit.map((ev) => `T${ev.tag} 提交`).join("；");
    text = [wbText, commitTextLine].filter(Boolean).join("；") + "。";
  } else if (issue.length) {
    title = "就绪指令进入执行单元";
    text = issue.map((ev) => `T${ev.tag} ${instLabel(ev)} 发射到 ${ev.fu}，预计执行 ${ev.latency} 个周期。`).join(" ");
  } else if (alloc.length) {
    title = "新指令被分配到 ROB 和保留站";
    text = alloc.map((ev) => {
      const wait = operandWaitText(ev, cycle);
      return `T${ev.tag} ${instLabel(ev)} 进入 ${ev.unit || ev.kind}${wait ? "，" + wait : "，源操作数已就绪"}。`;
    }).join(" ");
  } else if (active.length) {
    title = "长延迟指令仍在执行";
    text = active.map((item) => `T${item.tag} ${item.label} 正在 ${item.fu} 中执行，等待写回。`).join(" ");
  } else if (stall.length) {
    title = "流水线等待";
    text = stall.map((ev) => ev.reason).join("；") + "。";
  }

  els.storyTitle.textContent = title;
  els.storyText.textContent = text;
  const facts = [
    ["PC", meta ? hex(meta.pc) : "-"],
    ["ROB", meta ? `${meta.rob}/16` : "-"],
    ["保留站", meta ? `ALU ${meta.alu_rs}/8 · LS ${meta.ls_rs}/8 · BR ${meta.branch_rs}/4` : "-"],
    ["本周期事件", summarizeEvents(events)]
  ];
  els.storyFacts.innerHTML = facts.map(([name, value]) => `<div class="fact"><span>${name}</span><strong>${value}</strong></div>`).join("");
}

function renderProgramTable() {
  const rows = model.insts.map((item) => {
    const status = instStatusAt(item, currentCycle);
    const hot = itemHasEvent(item, currentCycle) ? "hot" : "";
    const explain = instExplain(item, currentCycle, status);
    return `<div class="program-row ${status.kind} ${hot}">
      <div><span class="tag-pill">T${item.tag}</span></div>
      <div><strong>${item.label}</strong><span>${[item.hint, item.pc != null ? "pc=" + hex(item.pc) : "", item.unit].filter(Boolean).join(" · ")}</span></div>
      <div><span class="state-pill state-${status.kind}">${status.label}</span></div>
      <div>${explain}</div>
    </div>`;
  });
  els.programTable.innerHTML = `<div class="program-row header"><div>Tag</div><div>指令</div><div>状态</div><div>为什么在这里</div></div>${rows.join("")}`;
}

function renderComponentFlow(cycle, events, meta) {
  const alloc = events.filter((ev) => ev.event === "allocate");
  const wb = events.filter((ev) => ev.event === "writeback");
  const commit = events.filter((ev) => ev.event === "commit");
  const robItems = model.insts.filter((item) =>
    item.allocCycle <= cycle &&
    (item.commitCycle == null || cycle < item.commitCycle) &&
    (item.flushCycle == null || cycle < item.flushCycle)
  );
  const rsItems = model.insts.filter((item) =>
    item.label !== "ecall" &&
    item.allocCycle <= cycle &&
    item.issueCycle == null &&
    (item.flushCycle == null || cycle < item.flushCycle)
  );
  const executing = model.insts.filter((item) =>
    item.issueCycle != null &&
    cycle >= item.issueCycle &&
    (item.wbCycle == null || cycle < item.wbCycle)
  );
  const waitingCommit = model.insts.filter((item) =>
    item.wbCycle != null &&
    cycle >= item.wbCycle &&
    (item.commitCycle == null || cycle < item.commitCycle)
  );
  const loadStore = model.insts.filter((item) =>
    item.unit === "LS" &&
    item.allocCycle <= cycle &&
    (item.commitCycle == null || cycle <= item.commitCycle)
  );

  const columns = [
    [
      component("PC / I-Cache", meta ? `PC=${hex(meta.pc)}` : "取指入口",
        alloc.length ? alloc.map((ev) => uop(ev.tag, instLabel(ev), `inst=${ev.inst != null ? hex(ev.inst, 8) : ""}`, "alloc")).join("") : emptyText("本周期未取新指令"),
        alloc.length),
      component("Decode / Rename", "分配 ROB tag，记录寄存器等待关系",
        alloc.length ? alloc.map((ev) => uop(ev.tag, instLabel(ev), renameText(ev), operandWaitText(ev, cycle) ? "waiting" : "alloc")).join("") : emptyText("没有新重命名"),
        alloc.length)
    ],
    [
      component("ROB", "按程序顺序提交",
        robItems.length ? `<div class="queue">${robItems.map((item, index) => `<span class="queue-token ${index === 0 ? "head" : ""}" title="${item.label}">T${item.tag}</span>`).join("")}</div>` : emptyText("空"),
        robItems.length),
      component("Commit", "只有 ROB head 可以更新架构状态",
        commit.length ? commit.map((ev) => uop(ev.tag, instLabel(ev), commitText(ev), "commit")).join("") : waitingCommit.length ? waitingCommit.slice(0, 3).map((item) => uop(item.tag, item.label, "已写回，等待更老指令提交", "done")).join("") : emptyText("本周期无提交"),
        commit.length)
    ],
    [
      component("Reservation Stations", "等待操作数；ready 后才能发射",
        rsItems.length ? rsItems.map((item) => uop(item.tag, item.label, operandWaitText(item, cycle) || "操作数已就绪，等执行单元", operandWaitText(item, cycle) ? "waiting" : "alloc")).join("") : emptyText("无等待指令"),
        rsItems.length),
      component("RegStat / Tags", "RAW/WAW 由 tag 跟踪",
        regStatText(cycle),
        rsItems.some((item) => operandWaitText(item, cycle)))
    ],
    [
      component("Functional Units", "ALU0 / ALU1 / LS / BR",
        executing.length ? executing.map((item) => uop(item.tag, `${item.fu || item.unit} · ${item.label}`, execRemainText(item, cycle), "executing")).join("") : emptyText("执行单元空闲"),
        executing.length),
      component("CDB Writeback", "广播 tag，唤醒等待操作数",
        wb.length ? wb.map((ev) => uop(ev.tag, instLabel(ev), writebackText(ev), "done")).join("") : emptyText("本周期无写回"),
        wb.length),
      component("Memory / RegFile", "load 从内存取值；commit 更新寄存器",
        memoryRegText(loadStore, commit, cycle),
        loadStore.length || commit.length)
    ]
  ];

  els.componentFlow.innerHTML = columns.map((cards, index) => {
    const col = `<div class="component-column">${cards.join("")}</div>`;
    return index === columns.length - 1 ? col : `${col}<div class="component-arrow">→</div>`;
  }).join("");
}

function component(title, subtitle, body, hot) {
  return `<div class="component-card ${hot ? "hot" : ""}">
    <div class="component-head"><strong>${title}</strong><span>${subtitle}</span></div>
    <div class="component-body">${body}</div>
  </div>`;
}

function uop(tag, title, detail, kind = "") {
  return `<div class="uop-card ${kind}"><strong>T${tag} · ${title}</strong><span>${detail || ""}</span></div>`;
}

function emptyText(text) {
  return `<div class="empty">${text}</div>`;
}

function renameText(ev) {
  const parts = [];
  if (ev.rd != null && ev.rd !== 0) parts.push(`${regName(ev.rd)} → T${ev.tag}`);
  if (ev.src1_ready === false) parts.push(`${regName(ev.rs1)} 等待 T${ev.src1_tag}`);
  if (ev.src2_ready === false) parts.push(`${regName(ev.rs2)} 等待 T${ev.src2_tag}`);
  return parts.length ? parts.join("；") : "源操作数 ready";
}

function regStatText(cycle) {
  const waits = model.insts
    .filter((item) => item.allocCycle <= cycle && item.issueCycle == null && operandWaitText(item, cycle))
    .map((item) => uop(item.tag, item.label, operandWaitText(item, cycle), "waiting"));
  if (waits.length) return waits.join("");
  const pendingWriters = model.insts
    .filter((item) => item.rd != null && item.rd !== 0 && item.allocCycle <= cycle && (item.commitCycle == null || cycle < item.commitCycle))
    .slice(-4)
    .map((item) => uop(item.tag, `${regName(item.rd)} 的当前生产者`, item.label, "alloc"));
  return pendingWriters.length ? pendingWriters.join("") : emptyText("当前没有未提交寄存器写者");
}

function execRemainText(item, cycle) {
  if (item.wbCycle == null) return `latency=${item.latency ?? "?"}`;
  const remain = Math.max(0, item.wbCycle - cycle);
  return `issue=${item.issueCycle}，预计 ${remain} cycle 后写回`;
}

function memoryRegText(loadStore, commit, cycle) {
  const cards = [];
  for (const item of loadStore.slice(-3)) {
    const status = instStatusAt(item, cycle).label;
    cards.push(uop(item.tag, item.label, `LS 状态：${status}`, "mem"));
  }
  for (const ev of commit) {
    if (ev.rd != null) cards.push(uop(ev.tag, `${regName(ev.rd)} 更新`, valueText(ev.value), "commit"));
    if (ev.store_addr != null) cards.push(uop(ev.tag, "Memory store", commitText(ev), "mem"));
  }
  return cards.length ? cards.join("") : emptyText("本周期无内存或寄存器更新");
}

function instStatusAt(item, cycle) {
  if (item.flushCycle != null && cycle >= item.flushCycle) return { kind: "flushed", label: "已 flush" };
  if (item.commitCycle != null && cycle >= item.commitCycle) return { kind: "committed", label: "已提交" };
  if (item.wbCycle != null && cycle >= item.wbCycle) return { kind: "done", label: "已写回" };
  if (item.issueCycle != null && cycle >= item.issueCycle) return { kind: "executing", label: "执行中" };
  if (item.allocCycle != null && cycle >= item.allocCycle) {
    if (item.label === "ecall") return { kind: "done", label: "等提交" };
    return operandWaitText(item, cycle) ? { kind: "waiting", label: "等操作数" } : { kind: "ready", label: "等发射" };
  }
  return { kind: "ready", label: "未分配" };
}

function instExplain(item, cycle, status) {
  if (cycle < item.allocCycle) return "还没有取到这条指令。";
  if (status.kind === "waiting") return operandWaitText(item, cycle);
  if (status.kind === "ready") return "已进入保留站，等待空闲执行单元。";
  if (status.kind === "executing") return `${item.fu || item.unit} 执行中，latency=${item.latency ?? "?"}。`;
  if (status.kind === "done") return "结果已经写回 ROB，但如果前面还有更老指令未提交，它还不能更新架构状态。";
  if (status.kind === "committed") return "ROB head 按程序顺序提交，架构寄存器/内存状态已经更新。";
  if (status.kind === "flushed") return "预测失败后被清除。";
  return "";
}

function operandWaitText(item, cycle = currentCycle) {
  const waits = [];
  if (item.src1Ready === false && item.src1Tag && !tagReadyAt(item.src1Tag, cycle)) waits.push(`src1 等待 T${item.src1Tag}`);
  if (item.src2Ready === false && item.src2Tag && !tagReadyAt(item.src2Tag, cycle)) waits.push(`src2 等待 T${item.src2Tag}`);
  return waits.join("，");
}

function tagReadyAt(tag, cycle) {
  const producer = model?.insts.find((item) => item.tag === Number(tag));
  return Boolean(producer && producer.wbCycle != null && cycle >= producer.wbCycle);
}

function itemHasEvent(item, cycle) {
  return [item.allocCycle, item.issueCycle, item.wbCycle, item.commitCycle, item.flushCycle].includes(cycle);
}

function summarizeEvents(events) {
  const names = [];
  if (events.some((ev) => ev.event === "allocate")) names.push("分配");
  if (events.some((ev) => ev.event === "issue")) names.push("发射");
  if (events.some((ev) => ev.event === "writeback")) names.push("写回");
  if (events.some((ev) => ev.event === "commit")) names.push("提交");
  if (events.some((ev) => ev.event === "stall")) names.push("停顿");
  if (events.some((ev) => ev.event === "flush")) names.push("Flush");
  return names.length ? names.join(" / ") : "无";
}

function chip(kind, title, body, foot) {
  return `<div class="chip ${kind}"><strong>${title}</strong><span>${body}</span>${foot ? `<span>${foot}</span>` : ""}</div>`;
}

function occupancy(name, value, cap) {
  const width = Math.min(100, Math.round((Number(value) / cap) * 100));
  return `<div class="chip"><strong>${name}: ${value}/${cap}</strong><span style="display:block;height:8px;border-radius:4px;background:#e3eaf0;margin-top:7px;overflow:hidden"><i style="display:block;width:${width}%;height:100%;background:var(--accent)"></i></span></div>`;
}

function writebackText(ev) {
  if (ev.store_addr != null) return `store ${hex(ev.store_addr)} <= ${valueText(ev.store_value)}`;
  if (ev.rd != null) return `${regName(ev.rd)} <= ${valueText(ev.value)}`;
  return "控制指令完成，无寄存器结果";
}

function commitText(ev) {
  if (ev.ecall) return "ecall";
  if (ev.store_addr != null) return `store ${hex(ev.store_addr)} <= ${valueText(ev.store_value)}`;
  if (ev.rd != null) return `${regName(ev.rd)} <= ${valueText(ev.value)}`;
  return `pc=${hex(ev.pc)}`;
}

function activeExecutions(cycle) {
  return model.insts.filter((item) =>
    item.issueCycle != null &&
    item.wbCycle != null &&
    cycle > item.issueCycle &&
    cycle < item.wbCycle
  );
}

function renderEvents(events) {
  els.eventList.innerHTML = events.length
    ? events.map((ev) => chip(flagClass(ev.event), ev.event, eventText(ev), JSON.stringify(ev))).join("")
    : `<div class="empty">本周期无事件</div>`;
}

function eventText(ev) {
  if (ev.inst != null || ev.asm) return `${ev.tag ? "T" + ev.tag + " · " : ""}${instLabel(ev)}`;
  if (ev.tag != null) return `T${ev.tag}`;
  if (ev.reason) return ev.reason;
  if (ev.pc != null) return `pc=${hex(ev.pc)}`;
  return "";
}

function renderDiagram(events) {
  const hot = new Set();
  for (const ev of events) {
    if (ev.event === "cycle") hot.add("pc");
    if (ev.event === "allocate") ["pc", "icache", "rename", "rs", "e1", "e2", "e3"].forEach((x) => hot.add(x));
    if (ev.event === "issue") ["rs", "fu", "e4"].forEach((x) => hot.add(x));
    if (ev.event === "writeback") ["fu", "cdb", "e5"].forEach((x) => hot.add(x));
    if (ev.event === "commit") ["rob", "regfile", "e7"].forEach((x) => hot.add(x));
    if (ev.event === "redirect" || ev.event === "flush") ["pc", "rob", "rename"].forEach((x) => hot.add(x));
  }
  els.diagram.querySelectorAll(".node,.edge").forEach((node) => {
    const keys = [...node.classList].filter((name) => name !== "node" && name !== "edge" && name !== "hot");
    node.classList.toggle("hot", keys.some((key) => hot.has(key)));
  });
}

function renderTimeline() {
  const columns = `repeat(${model.maxCycle + 1}, minmax(20px, 1fr))`;
  const header = `<div class="timeline-row"><div></div><div class="timeline-cells" style="grid-template-columns:${columns}">${Array.from({ length: model.maxCycle + 1 }, (_, i) => `<div class="cell ${i === currentCycle ? "active-cycle" : ""}" title="cycle ${i}">${i}</div>`).join("")}</div></div>`;
  const rows = model.insts.map((item) => {
    const cells = [];
    for (let c = 0; c <= model.maxCycle; c++) {
      cells.push(`<div class="cell ${cellClass(item, c)} ${c === currentCycle ? "active-cycle" : ""}" title="T${item.tag} cycle ${c}"></div>`);
    }
    return `<div class="timeline-row">
      <div class="timeline-name"><strong>T${item.tag} · ${item.unit || "?"}</strong><span>${item.label || "unknown"} · pc=${item.pc != null ? hex(item.pc) : "?"}</span></div>
      <div class="timeline-cells" style="grid-template-columns:${columns}">${cells.join("")}</div>
    </div>`;
  });
  els.timeline.innerHTML = `<div class="timeline-grid">${header}${rows.join("")}</div>`;
}

function cellClass(item, cycle) {
  if (item.flushCycle === cycle) return "flush";
  if (item.commitCycle === cycle) return "commit";
  if (item.wbCycle === cycle) return "wb";
  if (item.issueCycle != null && item.wbCycle != null && cycle >= item.issueCycle && cycle < item.wbCycle) return "issue";
  if (item.issueCycle === cycle) return "issue";
  if (item.allocCycle === cycle) return "alloc";
  return "";
}

function flagClass(eventName) {
  if (eventName === "allocate") return "alloc";
  if (eventName === "writeback") return "wb";
  if (eventName === "redirect") return "flush";
  return eventName;
}

function goToCycle(cycle) {
  currentCycle = Math.max(0, Math.min(model.maxCycle, cycle));
  renderCycle(currentCycle);
  renderTimeline();
}

function updateCycleRailActive() {
  els.cycleRail.querySelectorAll(".cycle-btn").forEach((button) => {
    button.classList.toggle("active", Number(button.dataset.cycle) === currentCycle);
  });
}

function togglePlay() {
  if (playTimer) {
    clearInterval(playTimer);
    playTimer = null;
    els.playBtn.textContent = "▶";
    return;
  }
  els.playBtn.textContent = "⏸";
  playTimer = setInterval(() => {
    if (currentCycle >= model.maxCycle) {
      togglePlay();
      return;
    }
    goToCycle(currentCycle + 1);
  }, 650);
}

function init() {
  Object.keys(PRESETS).forEach((name) => {
    const option = document.createElement("option");
    option.value = name;
    option.textContent = name;
    els.presetSelect.appendChild(option);
  });
  els.loadPresetBtn.addEventListener("click", () => setTrace(PRESETS[els.presetSelect.value]));
  els.prevBtn.addEventListener("click", () => goToCycle(currentCycle - 1));
  els.nextBtn.addEventListener("click", () => goToCycle(currentCycle + 1));
  els.playBtn.addEventListener("click", togglePlay);
  els.cycleSlider.addEventListener("input", () => goToCycle(Number(els.cycleSlider.value)));
  els.pasteLoadBtn.addEventListener("click", () => setTrace(els.traceInput.value));
  els.clearPasteBtn.addEventListener("click", () => { els.traceInput.value = ""; });
  els.traceFile.addEventListener("change", async () => {
    const file = els.traceFile.files[0];
    if (file) setTrace(await file.text());
  });
  setTrace(PRESETS[Object.keys(PRESETS)[0]]);
}

init();
