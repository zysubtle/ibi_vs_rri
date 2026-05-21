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

1. `ppg_ibi_context_t` 由 caller 分配，context 大小通过 `ppg_ibi_context_size()` 查询；
2. 当前 `ppg_ibi_context_size()` 为 **76 bytes**（以本分支 `make resource-report` 输出为准）；
3. 与 RAM 预算 15–20 KB 对比：`76 <= 20480`，处于预算内；
4. 库内不使用动态内存（无 `malloc/calloc/realloc`）；
5. 当前实现未引入不可控大栈数组与递归；
6. float 仍在允许范围内使用，但需持续关注无 FPU MCU 的运行时间与功耗风险。

## M8 检查方式

- `make test`：验证核心行为与状态机不回退；
- `make csv-smoke`：验证工程闭环输出；
- `make resource-report`：输出 `build/output/resource_report.txt`；
- 关键字扫描：确认无动态内存、无 `hr_bpm`/`rmssd`/`RMSSD` 输出字段；
- public header diff 检查：确认 `include/ppg_ibi.h` / `include/ppg_ibi_config.h` 无变更。

## resource report 字段约定

`build/output/resource_report.txt` 至少包含：

- `context_size_bytes=...`
- `ram_budget_min_bytes=15360`
- `ram_budget_max_bytes=20480`
- `context_size_within_budget=yes/no`
- `uses_dynamic_memory=no`

其中 `context_size_within_budget` 判定语义为：

- 当 `context_size_bytes <= ram_budget_max_bytes` 时为 `yes`；
- 否则为 `no`。

## M8 后仍需复盘的资源风险

1. **无 FPU + float**：当前规模可接受，但后续若引入更复杂 SQI/检测逻辑，需复盘 cycle 与功耗；
2. **RAM 增长风险**：后续若增加历史缓存窗口或多阶段 detector，需持续监控 context 与栈占用；
3. **50 Hz 采样约束**：若未来为准确性引入更多抗噪逻辑，需平衡资源预算与鲁棒性。
