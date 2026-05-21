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


## M5 资源结论补充

- `ppg_ibi_context_t` 增加最小 detector 历史字段，context 大小上升（可由 `ppg_ibi_context_size()` 读取）。
- 继续保持：无 `malloc/calloc/realloc`、无递归、无大栈数组。
- 算法仍以逐样本常数级计算为主；无 FPU 目标下 float 风险仍需后续里程碑持续复盘。
