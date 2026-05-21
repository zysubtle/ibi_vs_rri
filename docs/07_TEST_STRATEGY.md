# Test Strategy v0.5 (M8)

## 当前定位

当前无 ECG / 人工标注 / gold standard；测试目标是工程闭环，不是准确性证明。

## `make test` 组成

- API compile test
- input validation test
- signal quality test
- pulse detector test
- state machine test

## `make csv-smoke`

输入：`tests/fixtures/sample_ppg_20000.csv`（6 列：`timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4,allow_measure`）。

输出：

- `build/output/ibi_events.csv`
- `build/output/smoke_summary.txt`

summary 字段至少包括：

- `input_path`
- `total_samples`
- `parsed_samples`
- `invalid_lines`
- `allow_measure_false_samples`
- `event_ready_count`
- `ibi_min_ms`
- `ibi_max_ms`
- `ibi_out_of_range_events`
- 各 reject 计数与 `final_status`

## `make resource-report`

输出：`build/output/resource_report.txt`，字段包括：

- `context_size_bytes`
- `ram_budget_min_bytes`
- `ram_budget_max_bytes`
- `context_size_within_budget`
- `uses_dynamic_memory`

## 约束检查（必须执行）

- 动态内存扫描：`rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools`
- 禁止输出字段扫描：`rg -n "hr_bpm|rmssd|RMSSD" include src tests tools`
- public header diff：`git diff -- include/ppg_ibi.h include/ppg_ibi_config.h`

## 明确不做

在无 gold standard 情况下，不做 MAE / RMSE / matched beats / coverage 统计。

CSV smoke 仅用于工程可运行闭环，不用于临床或准确性结论。
