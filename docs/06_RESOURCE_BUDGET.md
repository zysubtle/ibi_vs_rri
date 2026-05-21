# Resource Budget v0.2

## 已确认预算

| 项目 | 预算 / 状态 |
|---|---|
| MCU | Apollo3.5 |
| FPU | Owner 当前声明无 FPU |
| RAM | 15–20 KB |
| ROM | 暂不考虑 |
| float | 允许（记录无 FPU 风险） |
| malloc/calloc/realloc | 禁止 |
| MISRA 风格 | 需要 |

## M2 资源冻结结论

1. `ppg_ibi_context_t` 由 caller 分配；
2. 库内不使用动态内存；
3. 通过 `ppg_ibi_context_size()` 暴露 context 大小供集成检查；
4. 当前 API 骨架不引入大栈数组与递归。

## M2 检查方式

- 编译并运行 `make test`；
- 运行关键字扫描，确认无 `malloc/calloc/realloc`。

## 风险记录

1. 无 FPU 但允许 float：后续实现阶段需关注运行时间与功耗；
2. RAM 15–20 KB：M3+ 引入缓冲后需持续复盘 context 增长；
3. 当前仅 API 骨架，尚未覆盖真实算法资源负载。

## M5 Fix 2 资源结论

- `ppg_ibi_context_t` 增加 detector history 与 last pulse 字段，RAM 增长为常数级。
- 仍为逐点 O(1) 计算，无动态内存、无递归、无大栈数组。
- 无 FPU 场景下未引入额外浮点密集计算。
