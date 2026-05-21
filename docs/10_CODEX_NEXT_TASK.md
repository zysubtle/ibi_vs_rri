# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M7：host 端 CSV smoke test、IBI event 导出与基础统计检查

## 背景

M2–M6 已完成：

1. 最小 MCU C API；
2. 逐点 `ppg_ibi_process()`；
3. 输入校验、timestamp / sample drop / 饱和标记；
4. 最小 SQI / selected_channel；
5. 最小三点 pulse candidate 与 IBI event 输出；
6. 状态机 / strict reject / REACQUIRE 语义收敛。

M7 的目标不是提升算法准确性，而是建立 host 端 CSV smoke test 闭环，验证算法能在仓库 fixture 上逐点运行并导出工程可检查结果。

## 重要前置条件

仓库中应存在：

```text
tests/fixtures/sample_ppg_20000.csv
```

该 CSV 的字段应为：

```text
timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4,allow_measure
```

如果该文件不存在，Codex 不得伪造大文件或随机生成同名 fixture。应优先报告：

```text
S1: missing tests/fixtures/sample_ppg_20000.csv
```

在文件缺失时，允许仍然实现工具和 Makefile target，但 `make csv-smoke` 必须给出清晰错误信息并非零退出；不得假装 M7 smoke test 通过。

## 本轮任务目标

实现 host 端 CSV smoke test 工具，读取仓库 fixture，逐点调用 MCU C API，并导出 IBI event 与 summary。

## 允许修改范围

允许修改：

```text
tools/ppg_ibi_csv_smoke.c
Makefile
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
docs/10_CODEX_NEXT_TASK.md
```

如必须新增目录，可新增：

```text
tools/
build/output/  # 运行时生成，不应提交构建产物
```

允许新增轻量说明文件：

```text
tools/README.md
```

## 禁止修改范围

默认不得修改：

```text
include/ppg_ibi.h
include/ppg_ibi_config.h
src/ppg_ibi.c
src/ppg_ibi_internal.h
tests/test_api_compile.c
tests/test_input_validation.c
tests/test_signal_quality.c
tests/test_pulse_detector.c
tests/test_state_machine.c
```

如果 Codex 认为必须修改算法源码或 public API，必须暂停并报告为 S0/S1，不得擅自修改。

## 工具设计要求

新增 host 工具建议命名：

```text
tools/ppg_ibi_csv_smoke.c
```

工具输入：

```text
tests/fixtures/sample_ppg_20000.csv
```

工具输出：

```text
build/output/ibi_events.csv
build/output/smoke_summary.txt
```

工具必须：

1. 使用 C 标准库实现；
2. 不使用 malloc / calloc / realloc；
3. 不引入第三方依赖；
4. 使用固定长度行缓冲；
5. 对 CSV header 做字段校验；
6. 逐行解析：
   - `timestamp_ms` → `uint32_t`
   - `PPG_G1..PPG_G4` → `int32_t`
   - `allow_measure` → `uint8_t`
7. 每行构造 `ppg_ibi_sample_t`；
8. 调用 `ppg_ibi_process()`；
9. 统计样本数、allow_measure false 样本数、状态 / reject reason；
10. 当 `ppg_ibi_process()` 返回 `PPG_IBI_STATUS_EVENT_READY` 时，将 event 写入 CSV；
11. 只检查 IBI 基本合理性，不做准确性评估。

## 输出 event CSV 要求

`build/output/ibi_events.csv` header 建议为：

```text
timestamp_ms,sample_index,ibi_ms,beat_count,confidence,signal_quality,selected_channel,state,reject_reason,debug_flags
```

每个 `EVENT_READY` 写一行。

不得输出：

```text
hr_bpm
rmssd
RMSSD
HRV
```

## summary 要求

`build/output/smoke_summary.txt` 至少包含：

```text
input_path
total_samples
parsed_samples
invalid_lines
allow_measure_false_samples
event_ready_count
ibi_min_ms
ibi_max_ms
ibi_out_of_range_events
reject_allow_measure_false
reject_low_signal_quality
reject_saturated
reject_timestamp_gap
reject_sample_drop
reject_ibi_out_of_range
final_status
```

如果无 event，`ibi_min_ms` / `ibi_max_ms` 可输出 `NA`。

## Makefile 要求

保留现有 `make test` 行为，并新增 target：

```text
make csv-smoke
```

建议：

```text
make csv-smoke
```

执行：

1. 编译 `tools/ppg_ibi_csv_smoke.c` + `src/ppg_ibi.c`；
2. 运行工具；
3. 输出到 `build/output/`。

可新增：

```text
make clean
```

继续清理 `build/`。

## 验收测试命令

Codex 必须运行并报告：

```bash
make test
make csv-smoke
rg -n "\b(malloc|calloc|realloc)\s*\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
```

如果 `tests/fixtures/sample_ppg_20000.csv` 不存在，则：

1. `make test` 仍应运行；
2. `make csv-smoke` 可以失败，但必须是明确的 missing fixture 错误；
3. Codex 必须在 Test results 中明确报告 fixture 缺失；
4. 不得生成假的 `sample_ppg_20000.csv`；
5. 不得判定 M7 完全通过。

## 通过标准

在 fixture 存在时，M7 通过标准：

1. `make test` 通过；
2. `make csv-smoke` 通过；
3. `build/output/ibi_events.csv` 被生成；
4. `build/output/smoke_summary.txt` 被生成；
5. summary 中 `parsed_samples > 0`；
6. 如果存在 event，则所有 `ibi_ms` 在 300–2000 ms；
7. `allow_measure=false` 时不应输出 event；
8. 未引入动态内存；
9. 未引入 HRV / RMSSD / HR 输出字段；
10. 未修改 public API。

## 非目标

本轮不做：

1. ECG / gold standard 对齐；
2. MAE / RMSE / matched beats / coverage；
3. 准确性宣称；
4. HRV / RMSSD 计算；
5. `hr_bpm` 输出；
6. GUI；
7. 第三方 PPG / IBI / HR / HRV 算法库；
8. Python 第三方依赖；
9. 算法增强或滤波优化。

## Codex 输出要求

完成后报告：

1. Summary；
2. Changed files；
3. 是否找到 `tests/fixtures/sample_ppg_20000.csv`；
4. Test commands；
5. Test results；
6. 输出文件路径；
7. event_ready_count；
8. IBI min/max；
9. Known limitations；
10. 是否修改 public API；
11. 是否引入动态内存；
12. 是否引入外部依赖；
13. 是否执行 Git 操作。

## Git 限制

Codex 不得：

```text
push
创建 PR
更新 PR
merge
rebase
删除远程分支
```

如执行本地 commit，必须报告 commit hash。


## M7 执行备注（Codex）

- 已实现 host CSV smoke 工具与 `make csv-smoke` 目标。
- 若后续进入 M8，请由 Owner 更新本文件任务内容。

