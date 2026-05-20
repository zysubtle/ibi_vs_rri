# Resource Budget v0.1

## 已确认预算

| 项目 | 预算 / 状态 |
|---|---|
| MCU | Apollo3.5 |
| FPU | Owner 当前声明无 FPU |
| RAM | 15–20 KB |
| ROM | 暂不考虑 |
| 单次 process 时间 | 暂不考虑 |
| 功耗 | 暂不考虑 |
| float | 允许 |
| malloc/calloc/realloc | 禁止 |
| MISRA 风格 | 需要 |

## RAM 原则

1. 算法 context 由 caller 分配；
2. 不使用动态内存；
3. 避免大数组上栈；
4. 缓存长度需在配置中集中定义；
5. M8 前需形成静态内存估算。

## 栈原则

1. 局部变量保持小规模；
2. 不在函数内定义大数组；
3. 不使用递归；
4. host 测试也不得隐藏大栈数组。

## float 风险

Owner 允许 float，但当前声明无 FPU，因此风险包括：

1. 单次 process 时间可能增加；
2. 功耗可能增加；
3. 后续可能需要定点化；
4. CMSIS-DSP 使用方式可能受限。

该风险不阻塞 M1，但应在 M8 复盘。

## M2 需冻结

1. context 结构体大小目标；
2. 是否允许静态内部缓存；
3. public API 是否暴露 context size；
4. 是否支持配置裁剪；
5. `confidence` / `signal_quality` 使用整数还是 float。
