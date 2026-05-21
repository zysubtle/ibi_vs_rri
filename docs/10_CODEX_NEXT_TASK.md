# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M6 Fix 1：补齐状态机 / 异常恢复测试覆盖

## 背景

M6 PR 已基本实现状态机、strict reject reset、EVENT_READY 字段对齐和基本可读性整理，但审查发现测试覆盖未完全满足 M6 任务文件的“至少覆盖”要求。

当前不要求重新设计算法。主要目标是补齐测试证明；如新增测试暴露行为缺陷，再做最小代码修复。

## 本轮任务目标

请在当前 M6 PR 分支上继续修复，优先补齐 `tests/test_state_machine.c` 的测试覆盖。

必须覆盖以下场景：

1. `TRACK + allow_measure=false`
   - 不返回 `EVENT_READY`；
   - 状态进入 `HOLD`；
   - `reject_reason=PPG_IBI_REJECT_ALLOW_MEASURE_FALSE`；
   - detector history 与 last pulse 被 reset。

2. `TRACK + TIMESTAMP_GAP`
   - 构造小于 expected interval 的 timestamp 异常；
   - 不返回 `EVENT_READY`；
   - 状态进入 `REACQUIRE`；
   - `reject_reason=PPG_IBI_REJECT_TIMESTAMP_GAP`；
   - detector history 与 last pulse 被 reset。

3. `TRACK + SAMPLE_DROP`
   - 构造大于 expected interval 的 timestamp 异常；
   - 不返回 `EVENT_READY`；
   - 状态进入 `REACQUIRE`；
   - `reject_reason=PPG_IBI_REJECT_SAMPLE_DROP`；
   - detector history 与 last pulse 被 reset。

4. `TRACK + LOW_SIGNAL_QUALITY`
   - 使用 near-saturation 但未达到 24-bit 边界的 low-quality 样本；
   - 不返回 `EVENT_READY`；
   - 状态进入 `REACQUIRE`；
   - `reject_reason=PPG_IBI_REJECT_LOW_SIGNAL_QUALITY`；
   - detector history 与 last pulse 被 reset。

5. `EVENT_READY` 后 history 连续性
   - 在一次合法 `EVENT_READY` 后，继续输入下一组合法 synthetic pulse；
   - 断言不会在下一拍立即重复输出同一 pulse；
   - 断言下一组合法 pulse 到达时才输出新的 `EVENT_READY`；
   - 新 event 的 `ibi_ms/timestamp_ms/sample_index/state/selected_channel/signal_quality/confidence` 必须合理且与 candidate 样本一致。

6. 继续保留已有覆盖：
   - `INIT -> ACQUIRE`；
   - `allow_measure=false -> HOLD`；
   - `HOLD -> REACQUIRE`；
   - 首个 candidate 只建立 last pulse，不输出 IBI；
   - 第二个合法 candidate 产生 `EVENT_READY + TRACK`；
   - `IBI_OUT_OF_RANGE` 不输出 event 且进入 `REACQUIRE` 或明确非 `TRACK` 状态。

## 允许修改范围

允许修改：

```text
tests/test_state_machine.c
tests/test_pulse_detector.c
src/ppg_ibi.c
Makefile
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

如果新增测试暴露行为缺陷，可对 `src/ppg_ibi.c` 做最小修复。

## 禁止修改范围

禁止修改：

```text
include/ppg_ibi_config.h
ppg_ibi_sample_t
ppg_ibi_event_t
public function signatures
ppg_ibi_status_t / ppg_ibi_state_t / ppg_ibi_reject_reason_t enum 名称或语义
采样率、通道数、IBI 范围常量
allow_measure=false 立即不输出 IBI 的语义
```

除非测试确实证明必须，否则不要修改 `include/ppg_ibi.h`。

## 非目标

本轮不做：

1. 不做滤波 / DC removal / 去趋势；
2. 不做滑动窗口 SQI；
3. 不做模板匹配、FFT、ACF、频域逻辑；
4. 不读取示例 CSV；
5. 不做真实数据准确性评估；
6. 不输出或计算 HR / HRV / RMSSD；
7. 不引入 CMSIS-DSP；
8. 不引入第三方 PPG / IBI / HR / HRV 算法库。

## 测试命令

必须执行并报告：

```bash
make test
rg -n "\b(malloc|calloc|realloc)\s*\(" include src tests
rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h
```

如 `rg` 不可用，可用等价 `grep -R -E`，但必须说明。

## 通过标准

本轮通过标准：

1. `make test` 通过；
2. 新增测试覆盖所有本任务目标中的 TRACK strict reject 场景；
3. 新增测试覆盖 EVENT_READY 后 history 连续性；
4. strict reject 后 detector history 和 last pulse 均被清理；
5. strict reject 样本不进入 detector prev/prev2 history；
6. 不修改 public function signatures、event 字段、enum、配置常量；
7. 不引入动态内存、外部依赖或第三方算法库；
8. 文档补充 M6 Fix 1 测试覆盖说明，不写成长篇论文。

## Codex 输出摘要要求

完成后请报告：

```text
Summary
Changed files
Test commands
Test results
Known limitations
是否修改 API：是/否
是否修改输出字段：是/否
是否修改 function signature：是/否
是否修改 status/state/reject enum：是/否
是否引入动态内存：是/否
是否引入外部依赖：是/否
是否执行 Git 操作：是/否
commit hash：xxx
```

## Git / PR 限制

请不要 push、不要创建 PR、不要 merge、不要 rebase。只允许本地修改、测试和本地 commit。Owner 自行处理 push 与 PR。
