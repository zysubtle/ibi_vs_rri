# Test Strategy v0.5 (M8)

## 当前定位

无 ECG / 人工标注 / gold standard。当前阶段测试目标是工程闭环验证，不宣称临床或总体准确性。

## `make test`（单元 + 行为）

`make test` 由以下测试组成：

1. API compile test（最小 C API 可调用、编译参数约束）；
2. input validation test（timestamp/sample counter/参数校验）；
3. signal quality test（基础 SQI / 饱和处理）；
4. pulse detector test（最小三点局部峰行为）；
5. state machine test（HOLD/REACQUIRE/TRACK 与 reject 语义）。

## `make csv-smoke`（CSV 工程闭环）

输入：

- `tests/fixtures/sample_ppg_20000.csv`

输出：

- `build/output/ibi_events.csv`
- `build/output/smoke_summary.txt`

`smoke_summary.txt` 需至少包含：

- `parsed_samples`
- `event_ready_count`
- `invalid_lines`
- `allow_measure_false_samples`
- `final_status`

说明：CSV smoke 用于验证输入→处理→输出链路可运行，不构成准确性证明。

## `make resource-report`（资源预算报告）

输出：

- `build/output/resource_report.txt`

报告字段至少包含：

- `context_size_bytes`
- `ram_budget_min_bytes`
- `ram_budget_max_bytes`
- `context_size_within_budget`
- `uses_dynamic_memory`

## 约束与回归检查（必须执行）

1. 动态内存扫描：
   - `rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools`
2. 禁止字段扫描（不输出 HR/RMSSD）：
   - `rg -n "hr_bpm|rmssd|RMSSD" include src tests tools`
3. public header diff 检查：
   - `git diff -- include/ppg_ibi.h include/ppg_ibi_config.h`

## 当前不做的准确性指标

在无 gold standard（ECG 或人工标注）前，不执行以下指标：

- MAE
- RMSE
- matched beats
- coverage

## 结论边界（M8）

- 通过 `make test` / `make csv-smoke` / `make resource-report` 仅能说明工程实现与约束检查通过。
- 不能据此宣称临床准确性或真实场景总体检测性能。
