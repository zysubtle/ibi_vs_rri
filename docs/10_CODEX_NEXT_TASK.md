# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M4：基础预处理、轻量 SQI 与主通道选择

## 本轮任务目标

在 M3 已完成基础输入路径的基础上，实现一个**不改变 public API** 的最小 SQI / 主通道选择路径。

本轮重点是让 no-event 输出中的以下字段具备稳定、可测试的基础含义：

1. `selected_channel`；
2. `signal_quality`；
3. `reject_reason` 中的 `LOW_SIGNAL_QUALITY`；
4. `debug_flags` 中的低质量 / 通道选择相关内部标记。

M4 仍然不输出真实 IBI。`ppg_ibi_process()` 仍必须返回 `PPG_IBI_STATUS_NO_EVENT`，`event.ibi_ms` 仍必须保持 0。

## 本轮非目标

本轮明确不做：

1. 不改 public API；
2. 不修改 public input/output struct 字段；
3. 不修改 public enum 名称或语义；
4. 不扩展 public `ppg_ibi_context_t` 字段；
5. 不实现需要历史 buffer 的 IIR/FIR/bandpass/DC removal；
6. 不做复杂 SQI；
7. 不做 FFT / ACF / 峰值检测；
8. 不计算真实 IBI；
9. 不返回 `PPG_IBI_STATUS_EVENT_READY`；
10. 不输出 `hr_bpm`；
11. 不输出 RMSSD / HRV；
12. 不读取示例 CSV；
13. 不实现 CLI / GUI / host 评估工具；
14. 不引入 CMSIS-DSP；
15. 不做 MCU 交叉编译。

说明：本轮的“基础预处理”仅限于 raw-range validity / channel score 这类逐样本、无历史缓存的轻量处理。真正滤波、DC removal、滑动窗口 SQI 等需要 context buffer 的工作留到后续里程碑或 S0 决策后再做。

## 允许修改 / 新增的文件

允许新增或修改：

```text
src/ppg_ibi.c
src/ppg_ibi_internal.h
src/ppg_ibi_signal.c
src/ppg_ibi_signal.h
tests/test_signal_quality.c
tests/test_input_validation.c
Makefile
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

说明：

1. 可以新增 `src/ppg_ibi_signal.c` / `src/ppg_ibi_signal.h` 作为内部 helper；
2. 可以只在 `src/ppg_ibi.c` 中新增 static helper，不强制拆分模块；
3. 可以新增 `tests/test_signal_quality.c`；
4. 可以更新 Makefile，让 `make test` 同时运行 M2、M3、M4 测试；
5. 可以小幅更新 `tests/test_input_validation.c`，但不得降低 M3 覆盖强度；
6. 文档更新保持简洁，只记录 M4 新增测试和风险处理。

## 禁止修改的文件 / 范围

本轮默认禁止修改：

```text
include/ppg_ibi.h
include/ppg_ibi_config.h
docs/00_PROJECT_BRIEF.md
docs/01_DECISION_LOG.md
docs/02_MILESTONE_PLAN.md
docs/03_ALGORITHM_SCOPE.md
docs/04_IO_CONTRACT.md
docs/05_MCU_ALGORITHM_STRATEGY.md
docs/06_RESOURCE_BUDGET.md
docs/09_CODEX_RUNBOOK.md
docs/11_GIT_WORKFLOW.md
AGENTS.md
```

如果 Codex 认为必须修改 `include/ppg_ibi.h`、`include/ppg_ibi_config.h` 或扩展 `ppg_ibi_context_t` 才能完成任务，应暂停并报告为 S0，不得擅自修改。

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

## M2/M3 已冻结行为，不得破坏

1. `ppg_ibi_process()` 每次处理 1 个样本；
2. `sample_counter` 从 1 开始递增；
3. `reset()` 后 sample index 重新从 1 开始；
4. `allow_measure==0` 时进入 `HOLD`，`reject_reason=ALLOW_MEASURE_FALSE`；
5. 从 `HOLD` 恢复 `allow_measure!=0` 后进入 `REACQUIRE`；
6. timestamp 正常 20ms 间隔不触发 reject；
7. timestamp 大于预期间隔标记 `SAMPLE_DROP` 或 sample-drop debug flag；
8. timestamp 其他异常标记 `TIMESTAMP_GAP` 或 timestamp-gap debug flag；
9. 24-bit PPG 边界 / 越界标记 `SATURATED` 或 saturated debug flag；
10. M4 仍不返回 `PPG_IBI_STATUS_EVENT_READY`；
11. M4 仍不产生有效 IBI，`ibi_ms == 0`。

## M4 实现要求

### 1. 最小 raw-range SQI

请实现逐样本、无历史缓存的通道质量评分。

建议评分规则如下，可以等价实现，但必须可测试、确定、无动态内存：

```text
若 raw <= -8,388,608 或 raw >= +8,388,607：channel_quality = 0，视为 saturated / invalid
若 raw 接近 24-bit 边界：channel_quality = 20，视为 low quality
否则：channel_quality = 80，视为 basic-valid quality
```

建议定义内部常量：

```text
PPG_IBI_PPG_NEAR_SATURATION_MARGIN
PPG_IBI_CHANNEL_QUALITY_INVALID = 0
PPG_IBI_CHANNEL_QUALITY_LOW = 20
PPG_IBI_CHANNEL_QUALITY_BASIC_VALID = 80
PPG_IBI_SIGNAL_QUALITY_ACCEPT_THRESHOLD = 50
PPG_IBI_SELECTED_CHANNEL_INVALID = 255
```

这些常量应放在内部文件，例如 `src/ppg_ibi_internal.h` 或内部 signal helper 中，不得放入 public header。

### 2. 主通道选择

在 `allow_measure != 0` 且样本未被门控关闭时，从 4 路 PPG 中选择质量分最高的通道：

1. 选择 `channel_quality` 最高的通道；
2. 若并列，选择 index 较小的通道；
3. 若全部通道 invalid，则 `selected_channel = 255`；
4. `event.signal_quality = best_channel_quality`；
5. M4 仍不输出 IBI，因此 `event.confidence` 保持 0。

### 3. 与 reject_reason 的关系

M4 建议保持简单优先级：

```text
ALLOW_MEASURE_FALSE > SATURATED > SAMPLE_DROP > TIMESTAMP_GAP > LOW_SIGNAL_QUALITY > NONE
```

要求：

1. `allow_measure == 0` 时，不需要计算 SQI / channel selection；
2. 若任一通道达到 24-bit 边界或越界，可继续保持 M3 的保守策略：`reject_reason = SATURATED`；
3. 即使存在饱和通道，也可以在 debug / no-event 输出中计算最佳非饱和通道，但不得输出 IBI；
4. 若没有更高优先级 reject，且 `best_channel_quality < PPG_IBI_SIGNAL_QUALITY_ACCEPT_THRESHOLD`，应设置 `PPG_IBI_REJECT_LOW_SIGNAL_QUALITY`；
5. 若 `best_channel_quality >= threshold` 且无其他异常，`reject_reason` 应保持 `PPG_IBI_REJECT_NONE`。

### 4. debug_flags

可在内部新增 debug flag，例如：

```text
PPG_IBI_DEBUG_FLAG_LOW_SIGNAL_QUALITY
PPG_IBI_DEBUG_FLAG_CHANNEL_SELECTED
```

要求：

1. 若成功选择了 0–3 的通道，可设置 CHANNEL_SELECTED flag；
2. 若低质量，可设置 LOW_SIGNAL_QUALITY flag；
3. debug flag 不得改变 public API。

### 5. 状态行为

M4 只补充 SQI / channel selection，不实现完整状态机：

1. 初始化后状态仍为 `INIT`；
2. 首次有效 allow_measure 样本后仍可进入 `ACQUIRE`；
3. allow_measure=false 仍进入 `HOLD`；
4. 从 HOLD 恢复 allow_measure=true 后仍进入 `REACQUIRE`；
5. SQI / channel selection 不应让算法返回 `EVENT_READY`。

## 测试要求

请新增或更新测试，使 `make test` 至少覆盖：

1. M2 API compile smoke test 仍通过；
2. M3 input validation test 仍通过；
3. allow_measure=false 时 `selected_channel=255`、`signal_quality=0`、`ibi_ms=0`；
4. allow_measure=true 且 4 路均 basic-valid 时，`selected_channel` 为确定通道，`signal_quality > 0`；
5. 当某些通道 near-saturation / low quality、某些通道 valid 时，选择质量更高的通道；
6. 当全部通道 low quality 但未越界时，触发 `LOW_SIGNAL_QUALITY` 或 low-quality debug flag；
7. 当任一通道 saturated / 越界时，仍触发 `SATURATED` 或 saturated debug flag；
8. M4 仍不返回 `PPG_IBI_STATUS_EVENT_READY`；
9. M4 仍不产生有效 IBI，`ibi_ms == 0`；
10. public headers 无 diff。

建议新增测试文件：

```text
tests/test_signal_quality.c
```

测试可以使用 C `assert`，不需要 Python。

## Makefile 要求

`make test` 必须：

1. 使用 C99；
2. 使用 `-Wall -Wextra -Werror`；
3. 编译并运行 M2 原有 API compile test；
4. 编译并运行 M3 input validation test；
5. 编译并运行 M4 signal quality / channel selection test；
6. 不依赖示例 CSV；
7. 不依赖 Python；
8. 不依赖外部库。

如果新增 `src/ppg_ibi_signal.c`，Makefile 必须把它纳入所有相关测试目标的编译。

## 文档更新要求

请更新：

```text
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

更新要求：

1. `docs/07_TEST_STRATEGY.md` 增加 M4 signal quality / channel selection test 说明；
2. `docs/08_RISK_REVIEW.md` 更新基础 SQI、主通道选择、无历史缓存、无真实滤波的当前处理状态；
3. 不要把文档写成长篇论文；
4. 保留长期测试策略，例如示例 CSV smoke test、无 gold standard 限制、Python 标准库规则。

## 禁止事项

本轮严禁：

1. 修改 public API；
2. 修改 public 输入 / 输出字段；
3. 修改 public enum 名称或语义；
4. 扩展 public `ppg_ibi_context_t` 字段；
5. `malloc` / `calloc` / `realloc`；
6. 第三方 PPG / IBI / HR / HRV 算法库；
7. Python 第三方库；
8. 真实 PPG 峰值检测；
9. 真实 IBI 计算；
10. 返回 `PPG_IBI_STATUS_EVENT_READY`；
11. 输出 `hr_bpm`；
12. 输出 RMSSD / HRV；
13. 引入 CMSIS-DSP；
14. 使用厂商 SDK 特殊数学函数；
15. 大数组上栈；
16. 递归；
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

必须额外检查 public header 未被修改：

```bash
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
```

如果 public header 发生变更，Codex 必须明确报告原因。除非任务明确授权，否则该情况应视为 S0 待 Owner 决策。

## 通过标准

M4 通过需要满足：

1. `make test` 通过；
2. M2 API compile smoke test 仍通过；
3. M3 input validation test 仍通过；
4. M4 signal quality / channel selection test 通过；
5. 未修改公开 API / 输出字段 / enum 语义；
6. 未扩展 public `ppg_ibi_context_t`；
7. `allow_measure=false` 行为正确；
8. allow_measure=true 时能得到确定的 `selected_channel` 与 `signal_quality`；
9. low-quality 与 saturated 场景有 reject 或 debug 标记；
10. 未输出真实 IBI；
11. 未返回 `PPG_IBI_STATUS_EVENT_READY`；
12. 未使用动态内存；
13. 未引入外部算法依赖；
14. 文档已更新到 M4 视角；
15. 没有执行远程 Git 操作。

## 失败时必须报告的信息

如果无法完成，Codex 必须报告：

1. 哪个文件无法创建或修改；
2. 哪条测试命令失败；
3. 失败日志；
4. 是否修改了 public API；
5. 是否扩展了 public context；
6. 是否引入了动态内存；
7. 是否引入了外部依赖；
8. 是否执行了任何 Git 操作；
9. 是否需要 Owner 决策；
10. 建议下一步修复点。

## Codex 输出摘要要求

完成后请输出：

```text
Summary
Changed files
Test commands
Test results
Known limitations
是否修改 API：是/否
是否修改输出字段：是/否
是否扩展 public context：是/否
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
