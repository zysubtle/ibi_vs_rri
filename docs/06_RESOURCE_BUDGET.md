# Resource Budget v0.3 (M8 收敛)

## 已确认预算

| 项目 | 预算 / 状态 |
|---|---|
| MCU | Apollo3.5 |
| FPU | Owner 当前声明无 FPU |
| RAM | 15–20 KB (15360–20480 bytes) |
| ROM | 暂不考虑 |
| float | 允许（记录无 FPU 风险） |
| malloc/calloc/realloc | 禁止 |
| MISRA 风格 | 需要 |

## M8 资源结论

1. 当前 `ppg_ibi_context_size()` 实测 `context_size_bytes=76`；
2. 判断规则：`context_size_bytes <= ram_budget_max_bytes` 即预算内；
3. 当前结果：`76 <= 20480`，因此 `context_size_within_budget=yes`；
4. 当前无动态内存（`uses_dynamic_memory=no`）；
5. 当前无递归（`uses_recursion=no`）；
6. 当前 context size 远低于 RAM 上限；
7. 当前未引入不可控大栈数组。

## 检查命令与输出

- `make resource-report`
- 输出文件：`build/output/resource_report.txt`
- 字段包含：
  - `context_size_bytes`
  - `ram_budget_min_bytes`
  - `ram_budget_max_bytes`
  - `context_size_within_budget`
  - `uses_dynamic_memory`
  - `uses_recursion`

## 风险与后续复盘

1. 无 FPU 但允许 float：后续真实负载下仍需复盘运行时间/功耗风险；
2. 后续若引入滑窗、更复杂 SQI、detector 缓存扩展，必须重新评估 RAM；
3. 当前结论仅表示工程预算通过，不代表算法准确性结论。
