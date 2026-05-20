# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M3：输入采样校验与基础数据路径

## 本轮任务目标

在 M2 已冻结最小 C API、输入 / 输出结构、状态枚举、reject reason 和资源约束的基础上，实现 `ppg_ibi_process()` 的基础输入数据路径与异常标记逻辑。

本轮重点是让逐点输入路径稳定、可测试、可审查：

1. `sample_counter` 递增行为；
2. timestamp 间隔检查；
3. timestamp gap / sample drop 标记；
4. 24-bit signed PPG raw 饱和 / 越界标记；
5. `allow_measure` 门控；
6. 无有效 IBI 时的 no-event 输出字段一致性；
7. 最小 host 测试覆盖。

M3 完成后，后续 M4 才开始考虑基础预处理、SQI 和主通道选择。

## 本轮非目标

本轮明确不做：

1. 不做 PPG 滤波；
2. 不做 SQI；
3. 不做主通道选择；
4. 不做脉搏峰检测；
5. 不计算真实 IBI；
6. 不输出真实 IBI event；
7. 不输出 `hr_bpm`；
8. 不输出 RMSSD / HRV；
9. 不读取示例 CSV；
10. 不实现 CLI / GUI / host 评估工具；
11. 不做 CMSIS-DSP 集成；
12. 不做 MCU 交叉编译；
13. 不改变公开 API / 输出字段 / enum 语义。

## 允许修改 / 新增的文件

允许新增或修改：

```text
src/ppg_ibi.c
src/ppg_ibi_internal.h
tests/test_api_compile.c
tests/test_input_validation.c
Makefile
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

说明：

1. 可以新增 `tests/test_input_validation.c`；
2. 可以在 `src/ppg_ibi.c` 中新增 static helper 函数；
3. 可以在 `src/ppg_ibi_internal.h` 中新增内部常量和 debug flag；
4. 可以更新 Makefile，让 `make test` 同时运行 M2 API compile test 和 M3 input validation test；
5. 文档更新应保持简洁，只记录 M3 新增测试和风险处理。

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

如果 Codex 认为必须修改 `include/ppg_ibi.h` 或 `include/ppg_ibi_config.h` 才能完成任务，应暂停并报告为 S0，不得擅自修改。

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
13. 可能饱和，可能丢样，无无效值标志；
14. 无 ACC；
15. 外部传入 `allow_measure`；
16. `allow_measure=false` 时立即停止输出 IBI；
17. 恢复 `allow_measure=true` 后进入 `REACQUIRE`；
18. 目标 MCU：Apollo3.5；
19. RAM 预算：15–20 KB；
20. 允许 float，但需记录无 FPU / 运行时间风险；
21. 禁止 `malloc/calloc/realloc`；
22. 需要 MISRA 风格限制；
23. 禁止第三方 PPG / IBI / HR / HRV 算法库；
24. 允许 C 标准库；
25. 允许 CMSIS-DSP，但本轮不得使用；
26. Python 仅可作为 host 测试辅助，且本轮不需要 Python。

## M2 已冻结 API，不得改变

M2 已冻结的 API 包括：

```c
void ppg_ibi_config_default(ppg_ibi_config_t *config);
ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx, const ppg_ibi_config_t *config);
ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx);
ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx,
                                  const ppg_ibi_sample_t *sample,
                                  ppg_ibi_event_t *event);
const char *ppg_ibi_version(void);
size_t ppg_ibi_context_size(void);
```

M3 不得改变函数签名、公开结构体字段、公开 enum 名称或公开常量语义。

## M3 实现要求

### 1. no-event 填充一致性

无有效 IBI 时，`ppg_ibi_process()` 仍返回 `PPG_IBI_STATUS_NO_EVENT`，并保证：

```text
ibi_ms = 0
beat_count 不增加
sample_index 与当前 sample_counter 一致
state 与当前内部状态一致
reject_reason 表示本样本主要拒绝原因或 NONE
debug_flags 表示本样本触发的异常标记
```

本轮不得返回 `PPG_IBI_STATUS_EVENT_READY`。

### 2. sample_counter 行为

每次成功传入非 NULL 且 ctx 已初始化的样本，应递增 sample counter。

建议测试：

1. 第 1 个样本输出 `sample_index == 1`；
2. 第 2 个样本输出 `sample_index == 2`；
3. `reset()` 后 sample index 重新从 1 开始。

### 3. allow_measure 门控

保持并测试以下行为：

1. `allow_measure == 0` 时立即停止输出 IBI；
2. 返回 `PPG_IBI_STATUS_NO_EVENT`；
3. `event.ibi_ms == 0`；
4. `event.reject_reason == PPG_IBI_REJECT_ALLOW_MEASURE_FALSE`；
5. `event.state == PPG_IBI_STATE_HOLD`；
6. debug flag 包含 allow_measure off 标记；
7. 从 HOLD 恢复到 `allow_measure != 0` 后，状态进入 `PPG_IBI_STATE_REACQUIRE`。

### 4. timestamp interval 检查

当 `config.allow_timestamp_strict_check != 0` 且已有上一帧 timestamp 时：

1. 相邻 timestamp 间隔等于 `expected_interval_ms` 时，不标记 timestamp reject；
2. timestamp 间隔小于或不等于预期间隔时，可标记 `PPG_IBI_REJECT_TIMESTAMP_GAP`；
3. timestamp 间隔大于预期间隔时，应优先标记 `PPG_IBI_REJECT_SAMPLE_DROP` 或至少在 debug flag 中标记 sample drop；
4. timestamp 反向、异常大跳变或疑似溢出时，应标记 `PPG_IBI_REJECT_TIMESTAMP_GAP` 或 `PPG_IBI_REJECT_SAMPLE_DROP`，不得输出 IBI。

如果同时出现多个异常，M3 可采用简单优先级：

```text
ALLOW_MEASURE_FALSE > SATURATED > SAMPLE_DROP > TIMESTAMP_GAP > NONE
```

优先级可以作为内部实现，不得改变公开 API。

### 5. PPG raw 饱和 / 越界检查

PPG raw 约束：

```text
24-bit signed effective range = -8,388,608 ~ +8,388,607
```

M3 至少需要检测：

1. 任一通道值小于 -8,388,608；
2. 任一通道值大于 +8,388,607；
3. 任一通道值等于 -8,388,608 或 +8,388,607，可保守视为饱和边界；
4. 出现饱和 / 越界时，不输出 IBI；
5. `reject_reason` 设置为 `PPG_IBI_REJECT_SATURATED`；
6. debug flag 包含饱和标记。

内部常量可放在 `src/ppg_ibi_internal.h` 或 `src/ppg_ibi.c`，不得为此修改 public header。

### 6. 状态行为

M3 只做基础状态行为，不实现完整状态机：

1. 初始化后状态为 `INIT`；
2. 首次有效 allow_measure 样本后可进入 `ACQUIRE`；
3. allow_measure=false 进入 `HOLD`；
4. 从 HOLD 恢复 allow_measure=true 后进入 `REACQUIRE`；
5. timestamp / saturation 等异常不应让算法返回 event ready。

如果需要更复杂状态设计，必须留到 M6，不得在 M3 提前实现完整状态机。

## 测试要求

请新增或更新测试，使 `make test` 至少覆盖：

1. M2 API compile smoke test 仍通过；
2. sample_counter 递增与 reset 后重置；
3. allow_measure=false no-event 行为；
4. allow_measure 从 false 恢复 true 后进入 REACQUIRE；
5. 正常 20 ms timestamp 不触发 timestamp reject；
6. timestamp 间隔异常触发 `TIMESTAMP_GAP` 或 `SAMPLE_DROP`；
7. timestamp 间隔大于 20 ms 时至少触发 sample drop 相关 reject 或 debug flag；
8. PPG raw 到达 24-bit signed 上下边界或越界时触发 `SATURATED`；
9. M3 仍不返回 `PPG_IBI_STATUS_EVENT_READY`；
10. M3 仍不产生有效 IBI，`ibi_ms == 0`。

建议新增测试文件：

```text
tests/test_input_validation.c
```

测试可以使用 C `assert`，不需要 Python。

## Makefile 要求

`make test` 必须：

1. 使用 C99；
2. 使用 `-Wall -Wextra -Werror`；
3. 编译并运行 M2 原有 API compile test；
4. 编译并运行 M3 input validation test；
5. 不依赖示例 CSV；
6. 不依赖 Python；
7. 不依赖外部库。

## 文档更新要求

请更新：

```text
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

更新要求：

1. `docs/07_TEST_STRATEGY.md` 增加 M3 input validation test 说明；
2. `docs/08_RISK_REVIEW.md` 更新 PPG 饱和、丢样、timestamp gap 的当前处理状态；
3. 不要把文档写成长篇论文；
4. 不要删除长期测试策略，例如示例 CSV smoke test、无 gold standard 限制、Python 标准库规则等内容；如前一版本已压缩，可在 v0.3 中补回简洁版长期规划。

## 禁止事项

本轮严禁：

1. 修改 public API；
2. 修改 public 输入 / 输出字段；
3. 修改 public enum 名称或语义；
4. `malloc` / `calloc` / `realloc`；
5. 第三方 PPG / IBI / HR / HRV 算法库；
6. Python 第三方库；
7. 真实 PPG 峰值检测；
8. 真实 IBI 计算；
9. 返回 `PPG_IBI_STATUS_EVENT_READY`；
10. 输出 `hr_bpm`；
11. 输出 RMSSD / HRV；
12. 引入 CMSIS-DSP；
13. 使用厂商 SDK 特殊数学函数；
14. 大数组上栈；
15. 递归；
16. push；
17. 创建 PR；
18. merge / rebase；
19. 删除远程分支。

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

建议额外检查 public header 未被修改：

```bash
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
```

如果 public header 发生变更，Codex 必须明确报告原因。除非任务明确授权，否则该情况应视为 S0 待 Owner 决策。

## 通过标准

M3 通过需要满足：

1. `make test` 通过；
2. M2 API compile smoke test 仍通过；
3. M3 input validation test 通过；
4. 未修改公开 API / 输出字段 / enum 语义；
5. `allow_measure=false` 行为正确；
6. 恢复 allow_measure 后进入 `REACQUIRE`；
7. sample_counter 行为可测试；
8. timestamp gap / sample drop 有 reject 或 debug 标记；
9. 24-bit signed PPG raw 饱和 / 越界有 reject 或 debug 标记；
10. 未输出真实 IBI；
11. 未返回 `PPG_IBI_STATUS_EVENT_READY`；
12. 未使用动态内存；
13. 未引入外部算法依赖；
14. 文档已更新到 M3 视角；
15. 没有执行远程 Git 操作。

## 失败时必须报告的信息

如果无法完成，Codex 必须报告：

1. 哪个文件无法创建或修改；
2. 哪条测试命令失败；
3. 失败日志；
4. 是否修改了 public API；
5. 是否引入了动态内存；
6. 是否引入了外部依赖；
7. 是否执行了任何 Git 操作；
8. 是否需要 Owner 决策；
9. 建议下一步修复点。

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
