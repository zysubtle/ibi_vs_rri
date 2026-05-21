# Resource Budget v0.3 (M8)

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

## M8 资源复盘结论

1. 当前 `ppg_ibi_context_size()` 实测：`context_size_bytes=76`（见 `make resource-report` 输出）。
2. RAM 预算窗口：`ram_budget_min_bytes=15360`、`ram_budget_max_bytes=20480`。
3. 预算判定规则：`context_size_bytes <= ram_budget_max_bytes` 即 `context_size_within_budget=yes`。
4. 当前 `76 <= 20480`，因此 `context_size_within_budget=yes`，且 context size 远低于 RAM 上限。
5. 当前无动态内存（无 `malloc/calloc/realloc`）。
6. 当前无递归；核心处理维持逐点 O(1) 路径。
7. 当前未引入不可控大栈数组。

## M8 检查方式

- 执行 `make resource-report`，读取 `build/output/resource_report.txt`：
  - `context_size_bytes`
  - `ram_budget_min_bytes`
  - `ram_budget_max_bytes`
  - `context_size_within_budget`
  - `uses_dynamic_memory`
  - `uses_recursion`
- 执行动态内存关键字扫描：
  - `rg -n "\b(malloc|calloc|realloc)\s*\(" include src tests tools`

## 资源风险与后续复盘

1. **无 FPU 但允许 float**：当前仍有无 FPU 下浮点开销风险，后续需结合目标 MCU 继续复盘运行时与功耗。
2. **RAM 风险前瞻**：若后续引入滑窗、复杂 SQI 或 detector 缓存，context 可能增长，必须重新评估 RAM 预算。
3. **当前阶段定位**：M8 仅做资源与工程收敛，不引入 HR/HRV/RMSSD 计算与新外部依赖。
