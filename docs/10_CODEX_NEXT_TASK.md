# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M5：脉搏候选检测与 IBI event 输出的最小实现

## 本轮任务目标

在 M4 已完成最小 SQI / 主通道选择的基础上，实现第一个可测试的自研脉搏候选检测与 IBI event 输出闭环。

本轮核心目标：

1. 基于 `selected_channel` 的 raw PPG 值，实现最小三点局部峰候选检测；
2. 在两个有效候选峰之间计算 `ibi_ms`；
3. 当 IBI 在 300–2000 ms 范围内时，输出 `PPG_IBI_STATUS_EVENT_READY`；
4. 有效事件中填充 `timestamp_ms`、`sample_index`、`ibi_ms`、`beat_count`、`confidence`、`signal_quality`、`selected_channel`、`state`、`reject_reason`、`debug_flags`；
5. 保持低质量、饱和、timestamp 异常、sample-drop、allow_measure=false 时不输出 IBI；
6. 不输出 HR、HRV、RMSSD。

M5 不是准确性评估里程碑。M5 只要求 synthetic host test 能验证工程闭环，不得宣称真实 PPG 数据上的准确性。

## 本轮受控 API / context 授权

M5 需要检测历史状态，因此本轮允许对 public header 做一个受控修改：

```text
include/ppg_ibi.h
```

只允许扩展 `ppg_ibi_context_t`，新增最小 detector 状态字段，例如：

```text
上一样本 / 上上一样本 raw；
上一样本 / 上上一样本 timestamp；
上一样本 / 上上一样本 sample_index；
上一样本 selected_channel；
last_pulse_timestamp_ms；
last_pulse_sample_index；
has_last_pulse；
beat_count / detector state 辅助字段；
```

严禁修改：

1. public function signatures；
2. `ppg_ibi_sample_t` 字段；
3. `ppg_ibi_event_t` 字段；
4. `ppg_ibi_status_t` enum；
5. `ppg_ibi_state_t` enum；
6. `ppg_ibi_reject_reason_t` enum；
7. 采样率、通道数、IBI 范围常量；
8. `allow_measure` 门控语义。

如果 Codex 认为必须修改上述禁止项，必须暂停并报告为 S0，不得擅自修改。

## 本轮非目标

本轮明确不做：

1. 不做 HRV / RMSSD 内部计算；
2. 不输出 `hr_bpm`；
3. 不返回任何 HR / HRV 字段；
4. 不做复杂滤波；
5. 不做 DC removal；
6. 不做滑动窗口 SQI；
7. 不做 FFT / ACF / 频域算法；
8. 不做模板匹配；
9. 不做机器学习 / 深度学习；
10. 不读取示例 CSV；
11. 不做真实数据准确性评估；
12. 不计算 MAE / RMSE / matched beats / coverage；
13. 不引入 CMSIS-DSP；
14. 不引入第三方 PPG / IBI / HR / HRV 算法库；
15. 不做 CLI / GUI / host 评估工具；
16. 不做 MCU 交叉编译；
17. 不做完整状态机重构。

## 允许修改 / 新增的文件

允许新增或修改：

```text
include/ppg_ibi.h
src/ppg_ibi.c
src/ppg_ibi_internal.h
src/ppg_ibi_detector.c
src/ppg_ibi_detector.h
tests/test_pulse_detector.c
tests/test_signal_quality.c
tests/test_input_validation.c
Makefile
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

说明：

1. `include/ppg_ibi.h` 只允许扩展 `ppg_ibi_context_t`；
2. 可以新增 `src/ppg_ibi_detector.c/.h`，也可以在 `src/ppg_ibi.c` 内用 static helper 实现；
3. 推荐新增 `tests/test_pulse_detector.c`；
4. Makefile 必须让 `make test` 同时运行 M2、M3、M4、M5 测试；
5. 文档更新保持简洁，只记录 M5 的最小 detector、context 扩展、测试和风险。

## 禁止修改的文件 / 范围

默认禁止修改：

```text
docs/00_PROJECT_BRIEF.md
docs/01_DECISION_LOG.md
docs/02_MILESTONE_PLAN.md
docs/03_ALGORITHM_SCOPE.md
docs/05_MCU_ALGORITHM_STRATEGY.md
docs/09_CODEX_RUNBOOK.md
docs/11_GIT_WORKFLOW.md
AGENTS.md
```

除 `ppg_ibi_context_t` 的受控扩展外，禁止修改 public API 和输出字段。

## 已确认项目约束

1. 目标：嵌入式 MCU 端 PPG-IBI 自研算法；
2. 用途：输出逐搏 IBI，供后续 HRV 使用；
3. 不做医疗诊断；
4. 不在算法内部计算 HRV / RMSSD；
5. 当前不输出 `hr_bpm`；
6. 输入：4 路同步绿光 PPG；
7. 采样率：50 Hz；
8. 调用方式：逐点 `process()`；
9. 每次输入：1 个样本，包含 4 路 PPG；
10. timestamp 单位：ms；
11. PPG raw：`int32_t`，24-bit signed；
12. 有效范围：-8,388,608 ~ +8,388,607；
13. 边界值和越界值可保守视为饱和；
14. 可能饱和，可能丢样，无无效值标志；
15. 无 ACC；
16. 外部传入 `allow_measure`；
17. `allow_measure=false` 时立即停止输出 IBI；
18. 恢复 `allow_measure=true` 后进入 `REACQUIRE`；
19. 目标 MCU：Apollo3.5；
20. RAM 预算：15–20 KB；
21. 允许 float，但需记录无 FPU / 运行时间风险；
22. 禁止 `malloc/calloc/realloc`；
23. 需要 MISRA 风格限制；
24. 禁止第三方 PPG / IBI / HR / HRV 算法库；
25. 允许 C 标准库；
26. 允许 CMSIS-DSP，但本轮不得使用；
27. Python 仅可作为 host 测试辅助，且本轮不需要 Python。

## M2/M3/M4 已冻结行为，不得破坏

1. `ppg_ibi_process()` 每次处理 1 个样本；
2. `sample_counter` 从 1 开始递增；
3. `reset()` 后 sample index 重新从 1 开始；
4. `allow_measure==0` 时进入 `HOLD`，`reject_reason=ALLOW_MEASURE_FALSE`；
5. 从 `HOLD` 恢复 `allow_measure!=0` 后进入 `REACQUIRE`；
6. timestamp 正常 20ms 间隔不触发 reject；
7. timestamp 大于预期间隔标记 `SAMPLE_DROP` 或 sample-drop debug flag；
8. timestamp 其他异常标记 `TIMESTAMP_GAP` 或 timestamp-gap debug flag；
9. 24-bit PPG 边界 / 越界标记 `SATURATED` 或 saturated debug flag；
10. `allow_measure=false` 时 `selected_channel=255`、`signal_quality=0`、`ibi_ms=0`；
11. allow_measure=true 时有确定的 `selected_channel` 和 `signal_quality`；
12. low-quality 与 saturated 场景有 reject 或 debug 标记；
13. M4 不输出真实 IBI，不返回 `EVENT_READY`。

## M5 推荐检测策略

请实现一个最小、确定、可测试、保守的候选峰检测策略。推荐但不强制的实现方式如下：

### 1. 三点局部峰候选

在 `allow_measure != 0`、当前样本无高优先级 reject、`selected_channel` 有效、`signal_quality >= threshold` 时，使用 selected channel 的 raw 值做三点局部峰检测。

建议规则：

```text
prev sample 是候选峰，当且仅当：
prev_raw > prev2_raw
prev_raw >= current_raw
```

说明：

1. 这是 1-sample latency 的候选检测；
2. 候选峰 timestamp 使用 prev sample 的 timestamp；
3. 候选峰 sample_index 使用 prev sample 的 sample_index；
4. 不要求真实 PPG 准确性，只要求 synthetic test 可稳定验证。

### 2. IBI 合法性

首次候选峰只记录为 `last_pulse`，不输出 IBI。

从第二个候选峰开始：

```text
ibi_ms = current_peak_timestamp_ms - last_peak_timestamp_ms
```

如果：

```text
300 <= ibi_ms <= 2000
```

则：

```text
return PPG_IBI_STATUS_EVENT_READY
```

并填充 event：

```text
event.timestamp_ms = current_peak_timestamp_ms
event.sample_index = current_peak_sample_index
event.ibi_ms = ibi_ms
event.beat_count = ctx->beat_count
event.confidence = event.signal_quality 或保守映射
event.state = PPG_IBI_STATE_TRACK
event.reject_reason = PPG_IBI_REJECT_NONE
```

如果 IBI 不在 300–2000 ms：

1. 不输出有效 event；
2. 返回 `PPG_IBI_STATUS_NO_EVENT`；
3. 可设置 `PPG_IBI_REJECT_IBI_OUT_OF_RANGE` 或 debug flag；
4. 可选择更新 last_peak 以便重新同步，但必须在测试中固定行为。

### 3. 门控与异常重置

以下场景不得输出 IBI，并建议清理 detector history，避免跨异常段产生虚假 IBI：

1. `allow_measure == 0`；
2. `SATURATED`；
3. `SAMPLE_DROP`；
4. `TIMESTAMP_GAP`；
5. `LOW_SIGNAL_QUALITY`；
6. `selected_channel == 255`。

`allow_measure=false` 必须继续进入 `HOLD`，恢复 true 后进入 `REACQUIRE`。

### 4. 状态行为

M5 只实现最小状态推进，不做完整 M6 状态机：

1. 初始 allow_measure=true 后仍进入 `ACQUIRE`；
2. 从 HOLD 恢复后进入 `REACQUIRE`；
3. 首次有效候选峰可以保持 ACQUIRE / REACQUIRE；
4. 产生有效 IBI event 后进入 `TRACK`；
5. 低质量、异常、门控 false 不输出 event。

## 测试要求

请新增或更新测试，使 `make test` 至少覆盖：

1. M2 API compile smoke test 仍通过；
2. M3 input validation test 仍通过；
3. M4 signal quality / channel selection test 仍通过；
4. 合成 PPG 序列中首次候选峰不输出 IBI；
5. 两个相隔 300–2000 ms 的候选峰输出一次 `PPG_IBI_STATUS_EVENT_READY`；
6. 输出 event 的 `ibi_ms` 与两个候选峰 timestamp 差一致；
7. 输出 event 的 `timestamp_ms` 与 `sample_index` 对应当前候选峰；
8. 输出 event 的 `beat_count` 递增；
9. 过短 IBI 不输出有效 event，且触发 `IBI_OUT_OF_RANGE` 或 debug 标记；
10. `allow_measure=false` 后不输出 IBI，并清理 detector history，避免恢复后跨门控段输出虚假 IBI；
11. saturated / timestamp gap / low quality 时不输出 event；
12. 不输出 `hr_bpm`、RMSSD、HRV；
13. 不使用动态内存；
14. public function signatures、sample/event/status/state/reject enum 不变。

建议新增测试文件：

```text
tests/test_pulse_detector.c
```

测试可以使用 C `assert`，不需要 Python，不读取示例 CSV。

## Makefile 要求

`make test` 必须：

1. 使用 C99；
2. 使用 `-Wall -Wextra -Werror`；
3. 编译并运行 M2 原有 API compile test；
4. 编译并运行 M3 input validation test；
5. 编译并运行 M4 signal quality / channel selection test；
6. 编译并运行 M5 pulse detector test；
7. 不依赖示例 CSV；
8. 不依赖 Python；
9. 不依赖外部库。

如果新增 `src/ppg_ibi_detector.c`，Makefile 必须把它纳入所有相关测试目标的编译。

## 文档更新要求

请更新：

```text
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

更新要求：

1. `docs/04_IO_CONTRACT.md` 记录 M5 允许的 context 扩展与 EVENT_READY 语义；
2. `docs/06_RESOURCE_BUDGET.md` 记录 context size 增长、仍无动态内存、无大栈数组；
3. `docs/07_TEST_STRATEGY.md` 增加 M5 synthetic pulse detector test 说明；
4. `docs/08_RISK_REVIEW.md` 增加最小 detector 的局限：synthetic-only、无滤波、无 gold standard、可能误检，后续 M6/M7 需复盘；
5. 不要把文档写成长篇论文；
6. 保留长期测试策略，例如示例 CSV smoke test、无 gold standard 限制、Python 标准库规则。

## 禁止事项

本轮严禁：

1. 修改 public function signatures；
2. 修改 public 输入 / 输出结构体字段；
3. 修改 public status/state/reject enum 名称或语义；
4. 输出 `hr_bpm`；
5. 输出 RMSSD / HRV；
6. 引入第三方 PPG / IBI / HR / HRV 算法库；
7. 引入 CMSIS-DSP；
8. 使用厂商 SDK 特殊数学函数；
9. 使用 `malloc/calloc/realloc`；
10. 使用动态容器；
11. 使用不可控大栈数组；
12. 使用递归；
13. 使用 Python 第三方库；
14. 读取示例 CSV；
15. 做真实数据准确性评估；
16. 宣称 HRV 准确性；
17. push；
18. 创建 PR；
19. merge / rebase；
20. 删除远程分支。

## 测试命令

Codex 必须执行并报告：

```bash
make test
```

并执行以下检查，或说明等价检查方式：

```bash
! grep -R -E "\b(malloc|calloc|realloc)\s*\(" include src tests
! grep -R -E "hr_bpm|rmssd|RMSSD" include src tests
```

必须额外检查 public function signatures 和输出结构体是否未被修改。建议至少执行：

```bash
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
```

如果 `include/ppg_ibi.h` 发生变更，Codex 必须明确说明变更仅限 `ppg_ibi_context_t` 扩展，并列出新增字段。

## 通过标准

M5 通过需要满足：

1. `make test` 通过；
2. M2/M3/M4 既有测试仍通过；
3. M5 synthetic pulse detector test 通过；
4. 能在 synthetic sequence 中输出至少一个 `PPG_IBI_STATUS_EVENT_READY`；
5. event 的 `ibi_ms` 在 300–2000 ms；
6. 首次候选峰不输出 IBI；
7. 过短 / 过长 IBI 不输出有效 event；
8. `allow_measure=false` 不输出 IBI，并避免跨门控段输出虚假 IBI；
9. saturated / timestamp gap / low quality 不输出 event；
10. 未输出 HR / HRV / RMSSD；
11. 未修改 sample/event/status/state/reject enum；
12. 仅对 `ppg_ibi_context_t` 做受控扩展；
13. 未使用动态内存；
14. 未引入外部算法依赖；
15. 文档已更新到 M5 视角；
16. 没有执行远程 Git 操作。

## 失败时必须报告的信息

如果无法完成，Codex 必须报告：

1. 哪个文件无法创建或修改；
2. 哪条测试命令失败；
3. 失败日志；
4. 是否修改了 public function signature；
5. 是否修改了 sample/event/status/state/reject enum；
6. 是否扩展了 public context，新增了哪些字段；
7. 是否引入了动态内存；
8. 是否引入了外部依赖；
9. 是否执行了任何 Git 操作；
10. 是否需要 Owner 决策；
11. 建议下一步修复点。

## Codex 输出摘要要求

完成后请输出：

```text
Summary
Changed files
Test commands
Test results
Known limitations
是否修改 API：是/否；若是，是否仅限 ppg_ibi_context_t 扩展
是否修改输出字段：是/否
是否修改 function signature：是/否
是否修改 status/state/reject enum：是/否
是否引入动态内存：是/否
是否引入外部依赖：是/否
是否执行 Git 操作：是/否
```

## Git / PR 限制

Codex 不得：

1. push；
2. 创建 PR；
3. 更新 PR；
4. merge；
5. rebase；
6. 删除远程分支。

Owner 自行创建分支、push、创建 PR 和合并 PR。
