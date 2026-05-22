# Test Strategy v0.8 (M8 Final Consolidation)

## 当前定位

无 ECG / 人工标注 / gold standard，当前阶段做工程闭环验证与契约回归，不宣称临床准确性。

## `make test` 组成

1. API compile test；
2. input validation test；
3. signal quality test；
4. pulse detector test；
5. state machine test。

## `make csv-smoke`

- 输入：`tests/fixtures/sample_ppg_20000.csv`
- 输出：`build/output/ibi_events.csv`
- 输出：`build/output/smoke_summary.txt`
- `smoke_summary.txt` 至少包含：
  - `parsed_samples`
  - `invalid_lines`
  - `allow_measure_false_samples`
  - `event_ready_count`
  - `final_status`

说明：CSV smoke 仅证明工程闭环（读入->process->导出），不证明准确性。

## `make resource-report`

- 输出：`build/output/resource_report.txt`
- 关键字段至少包含：
  - `context_size_bytes`
  - `ram_budget_min_bytes`
  - `ram_budget_max_bytes`
  - `context_size_within_budget`
  - `uses_dynamic_memory`
  - `uses_recursion`

## 约束与守护检查

1. 动态内存扫描：
   - `rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools`
2. 禁止输出字段扫描：
   - `rg -n "hr_bpm|rmssd|RMSSD" include src tests tools`
3. public header diff 检查：
   - `git diff -- include/ppg_ibi.h include/ppg_ibi_config.h`

## 准确性边界

1. 在无 gold standard 条件下，不执行 MAE / RMSE / matched beats / coverage；
2. `EVENT_READY` 与 CSV smoke 结果仅代表工程闭环，不代表临床准确性或医疗可用性。
