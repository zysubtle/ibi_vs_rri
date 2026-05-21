# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M5 修复轮 1：修复最小脉搏候选检测的跨通道历史混用与事件字段一致性问题。

## 背景

M5 PR 已实现三点局部峰检测、IBI 合法性检查和 `EVENT_READY` 输出闭环，但审查发现两个需要修复的问题：

1. 当前 detector 只保存 `prev_selected_channel`，没有保存 `prev2_selected_channel`，三点峰比较时可能把 `prev2_raw`、`prev_raw`、`current_raw` 来自不同 selected channel 的值混在一起比较，导致跨通道伪峰。
2. 当前 `EVENT_READY` 事件的 `timestamp_ms` / `sample_index` 指向被确认的前一样本峰值，但 `signal_quality` / `selected_channel` 仍来自当前确认样本。事件字段应尽量对应同一个 pulse candidate 样本。

本轮目标是最小修复，不扩大算法范围。

## 本轮任务目标

1. 修复三点局部峰检测的跨通道历史混用问题。
2. 确保只有当三点历史属于同一 selected channel 时，才允许进行局部峰候选判定；或者在 selected channel 发生变化时重置 detector history。
3. 让 `EVENT_READY` 输出中的 `selected_channel` 和 `signal_quality` 对应被确认的 pulse candidate 样本，而不是当前确认样本。
4. 增加针对 selected channel 切换的 synthetic test，确保不会因跨通道 raw 值混用而产生伪 IBI event。
5. 保持 M5 原有行为：无动态内存、无第三方库、不输出 `hr_bpm` / HRV / RMSSD、不修改 function signature、不修改 `ppg_ibi_event_t` 字段、不修改状态 / reject / status enum。

## 允许修改 / 新增的文件

允许修改：

```text
include/ppg_ibi.h
src/ppg_ibi.c
tests/test_pulse_detector.c
Makefile
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

说明：

1. `include/ppg_ibi.h` 仍只允许扩展 `ppg_ibi_context_t` 的 detector history 字段。
2. 如需新增 `prev2_selected_channel`、`prev_signal_quality`、`prev2_signal_quality` 等最小历史字段，可以新增。
3. 不得修改 `ppg_ibi_sample_t`、`ppg_ibi_event_t`、function signatures、status/state/reject enum。
4. 不得修改 `include/ppg_ibi_config.h`。

## 禁止事项

本轮严禁：

1. 引入滤波、DC removal、滑动窗口 SQI、FFT/ACF、模板匹配、机器学习；
2. 读取示例 CSV；
3. 做真实数据准确性评估；
4. 输出 `hr_bpm`、HRV、RMSSD；
5. 引入 `malloc/calloc/realloc`；
6. 引入第三方 PPG / IBI / HR / HRV 算法库；
7. 引入 CMSIS-DSP；
8. 修改 public function signatures；
9. 修改 `ppg_ibi_event_t` 输出字段；
10. 执行 push、创建 PR、merge、rebase 或任何远程 Git 操作。

## 建议实现方式

可选方案 A：增加历史 selected channel 字段。

```text
prev2_selected_channel
prev_signal_quality
prev2_signal_quality
```

局部峰判定时要求：

```text
prev2_selected_channel == prev_selected_channel
prev_selected_channel == current selected_channel
```

只有满足同一通道连续三点时，才比较：

```text
prev_raw > prev2_raw && prev_raw >= current_raw
```

输出 `EVENT_READY` 时：

```text
event.timestamp_ms = prev_timestamp_ms
event.sample_index = prev_sample_index
event.selected_channel = prev_selected_channel
event.signal_quality = prev_signal_quality
event.confidence = prev_signal_quality
```

可选方案 B：selected channel 变化时直接重置 detector history。

若采用方案 B，需要确保不会因 channel 切换产生候选峰或 IBI event。

## 测试要求

更新或新增 `tests/test_pulse_detector.c`，至少覆盖：

1. 原有 M5 测试仍通过：首个候选不出 IBI，合法 IBI 触发 `EVENT_READY`，过短 IBI reject，门控/饱和/timestamp 异常/低质量不出 event。
2. 新增 selected channel 切换测试：构造连续样本让 selected channel 在三点窗口内发生变化，即使 raw 数值看起来满足局部峰形态，也不得返回 `PPG_IBI_STATUS_EVENT_READY`。
3. 新增事件字段一致性测试：合法 event 的 `timestamp_ms`、`sample_index`、`selected_channel`、`signal_quality` 应对应同一个被确认的 pulse candidate 样本。
4. `make test` 必须通过。

## 必须执行的检查命令

```bash
make test
! rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests
! rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h
```

若环境没有 `rg`，可用等价 `grep -R -E` 命令替代，并在输出中说明。

## 通过标准

1. `make test` 通过；
2. selected channel 切换不会产生跨通道伪 IBI event；
3. 合法 `EVENT_READY` 的事件字段与被确认的 pulse candidate 样本一致；
4. 仅允许 `ppg_ibi_context_t` 扩展 detector history 字段；
5. 不修改 `ppg_ibi_event_t`、function signatures、status/state/reject enum；
6. 不引入动态内存、外部依赖、HR/HRV/RMSSD 输出；
7. 文档只做简洁补充，不扩大里程碑范围。

## Codex 输出要求

完成后请报告：

1. Summary；
2. Changed files；
3. Test commands；
4. Test results；
5. 是否修改 API；
6. 是否只扩展 `ppg_ibi_context_t`；
7. 是否修改输出字段；
8. 是否引入动态内存；
9. 是否引入外部依赖；
10. 是否执行任何 Git 操作。
