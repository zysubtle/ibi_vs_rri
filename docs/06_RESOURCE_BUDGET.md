# Resource Budget v0.8 (M8 Final Consolidation)

## 已确认预算

| 项目 | 预算 / 状态 |
|---|---|
| MCU | Apollo3.5 |
| FPU | Owner 当前声明无 FPU |
| RAM | 15–20 KB |
| ROM | 暂不考虑 |
| float | 允许（需持续记录无 FPU 风险） |
| malloc/calloc/realloc | 禁止 |
| MISRA 风格 | 需要 |

## M8 资源收敛结论

1. `ppg_ibi_context_t` 由 caller 分配；
2. 库内无动态内存、无递归；
3. 提供 `ppg_ibi_context_size()` 供资源审计；
4. `context_size_bytes=76`；
5. `context_size_within_budget=yes`（76 <= 20480）。

## 资源检查命令

- `make resource-report`
- 输出文件：`build/output/resource_report.txt`

关键输出字段：

- `context_size_bytes`
- `ram_budget_min_bytes`
- `ram_budget_max_bytes`
- `context_size_within_budget`
- `uses_dynamic_memory`
- `uses_recursion`

## 风险记录

1. 无 FPU 但允许 float：后续真实工作负载下仍需复盘运行时间/功耗；
2. RAM 预算当前充足，但后续若增加缓存或更复杂 detector 仍需持续评估；
3. 当前报告聚焦 context RAM，不覆盖完整系统峰值 RAM/ROM。
