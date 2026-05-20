# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M2：IO Contract、MCU C API 与资源约束冻结

## 本轮任务目标

在不实现 PPG-IBI 检测算法的前提下，冻结本项目的最小 C API、输入 / 输出数据结构、状态枚举、reject reason、context 分配方式和资源约束，并建立可编译的 MCU C API 骨架。

M2 完成后，后续 M3–M6 可以基于稳定 API 继续实现输入校验、预处理、SQI、峰值检测、状态机和 smoke test。

## 本轮非目标

本轮明确不做：

1. 不做 PPG 滤波；
2. 不做 SQI；
3. 不做主通道选择；
4. 不做峰值检测；
5. 不计算真实 IBI；
6. 不输出真实 HR / HRV / RMSSD；
7. 不读取示例 CSV；
8. 不实现 CLI / GUI / host 评估工具；
9. 不做 CMSIS-DSP 集成；
10. 不做 MCU 交叉编译。

## 允许修改 / 新增的文件

允许新增或修改：

```text
include/ppg_ibi.h
include/ppg_ibi_config.h
src/ppg_ibi.c
src/ppg_ibi_internal.h
tests/test_api_compile.c
Makefile
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

如果仓库缺少 `include/`、`src/` 或 `tests/` 目录，可以创建。

## 禁止修改的文件 / 范围

除非为保持文档引用一致而做极小更新，否则不要修改：

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

禁止删除 M1 文档。

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

## M2 需要冻结的 C API

请生成最小公开 API，建议放在：

```text
include/ppg_ibi.h
include/ppg_ibi_config.h
```

建议至少包含以下接口。Codex 可以在不改变语义的前提下微调命名，但不得改变本节定义的字段和约束。

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

API 语义：

1. context 由 caller 分配；
2. 库内部不得调用动态内存；
3. `ppg_ibi_process()` 每次处理 1 个样本；
4. 本轮 `ppg_ibi_process()` 只做占位行为，不检测真实 IBI；
5. 本轮不得输出真实有效 IBI event；
6. 如果 `allow_measure == 0`，应返回 no-event，并设置状态 / reject reason 为门控相关值；
7. 如果 timestamp 间隔不是 20 ms，可记录 timestamp gap reject reason，但不要实现复杂丢样恢复；
8. 所有 public API 对 `NULL` 参数应有确定返回值，不得崩溃。

## M2 需要冻结的输入结构

请在公开 header 中定义输入样本结构，字段语义如下：

```text
timestamp_ms: uint32_t，单位 ms
ppg[4]: int32_t[4]，4 路同步 PPG raw
allow_measure: uint8_t，0=false，非 0=true
```

固定常量：

```text
PPG_IBI_CHANNEL_COUNT = 4
PPG_IBI_SAMPLE_RATE_HZ = 50
PPG_IBI_EXPECTED_INTERVAL_MS = 20
PPG_IBI_MIN_IBI_MS = 300
PPG_IBI_MAX_IBI_MS = 2000
```

## M2 需要冻结的输出结构

请定义 `ppg_ibi_event_t`，至少包含以下字段：

```text
timestamp_ms: uint32_t
sample_index: uint32_t
ibi_ms: uint16_t
beat_count: uint32_t
confidence: uint8_t，0–100
signal_quality: uint8_t，0–100
selected_channel: uint8_t，0–3 有效，255 表示 invalid
state: ppg_ibi_state_t
reject_reason: ppg_ibi_reject_reason_t
debug_flags: uint32_t
```

说明：

1. 当前不输出 `hr_bpm`；
2. 当前不输出 RMSSD / HRV；
3. `ppg_ibi_process()` 的返回状态用于区分是否有有效 IBI event；
4. 当没有有效 IBI 时，`ibi_ms` 应为 0，其他状态 / reject 信息可用于调试；
5. `confidence` 和 `signal_quality` 统一使用 0–100 整数，避免在公开输出中使用 float。

## M2 需要冻结的状态枚举

请使用带项目前缀的 C enum，至少包含：

```text
PPG_IBI_STATE_INIT
PPG_IBI_STATE_ACQUIRE
PPG_IBI_STATE_TRACK
PPG_IBI_STATE_HOLD
PPG_IBI_STATE_REACQUIRE
PPG_IBI_STATE_INVALID
```

M2 只需定义枚举和占位状态迁移，不实现完整状态机。

## M2 需要冻结的 reject reason 枚举

请使用带项目前缀的 C enum，至少包含：

```text
PPG_IBI_REJECT_NONE
PPG_IBI_REJECT_ALLOW_MEASURE_FALSE
PPG_IBI_REJECT_LOW_SIGNAL_QUALITY
PPG_IBI_REJECT_SATURATED
PPG_IBI_REJECT_TIMESTAMP_GAP
PPG_IBI_REJECT_SAMPLE_DROP
PPG_IBI_REJECT_CHANNEL_INVALID
PPG_IBI_REJECT_IBI_OUT_OF_RANGE
PPG_IBI_REJECT_NO_STABLE_PULSE
PPG_IBI_REJECT_INTERNAL_INVALID
```

M2 只需定义枚举；不需要实现所有 reject 场景。

## M2 需要冻结的 status 返回值

请定义 `ppg_ibi_status_t`，至少支持：

```text
PPG_IBI_STATUS_OK
PPG_IBI_STATUS_NO_EVENT
PPG_IBI_STATUS_EVENT_READY
PPG_IBI_STATUS_INVALID_ARGUMENT
PPG_IBI_STATUS_NOT_INITIALIZED
```

M2 占位实现通常返回 `NO_EVENT`，不得伪造真实 IBI。

## 配置结构要求

请定义 `ppg_ibi_config_t`，至少包含：

```text
sample_rate_hz: uint16_t，默认 50
expected_interval_ms: uint16_t，默认 20
min_ibi_ms: uint16_t，默认 300
max_ibi_ms: uint16_t，默认 2000
allow_timestamp_strict_check: uint8_t，默认 1
```

配置原则：

1. 默认配置必须与 Project Brief 一致；
2. 如果 config 为 NULL，`ppg_ibi_init()` 应使用默认配置；
3. 不要引入复杂参数；
4. 参数应集中在 config 中，避免魔法数散落。

## 占位实现要求

`src/ppg_ibi.c` 只需实现 API 骨架：

1. `init` 初始化 context；
2. `reset` 复位 context；
3. `config_default` 填默认值；
4. `version` 返回固定字符串，例如 `"0.2.0-m2"`；
5. `context_size` 返回 `sizeof(ppg_ibi_context_t)`；
6. `process` 做参数检查、sample counter 更新、timestamp 间隔检查和 allow_measure 门控；
7. `process` 不做滤波、不做 SQI、不做峰值检测、不输出真实 IBI。

## Makefile / 测试要求

如果仓库尚无 Makefile，请新增最小 Makefile。

必须支持：

```bash
make test
```

`make test` 至少应：

1. 用 C99 编译 `tests/test_api_compile.c` 与 `src/ppg_ibi.c`；
2. 使用 `-Wall -Wextra -Werror`；
3. 运行测试可执行文件；
4. 测试默认配置、init/reset/process 基本调用；
5. 验证 `ppg_ibi_context_size() > 0`；
6. 验证 allow_measure=false 不产生 event；
7. 验证 API 可被外部 C 文件 include。

测试不得需要示例 CSV。

## 文档更新要求

请更新：

```text
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

更新要求：

1. `docs/04_IO_CONTRACT.md` 升级为 v0.2，记录 M2 冻结的字段类型、API、状态枚举和 reject reason；
2. `docs/06_RESOURCE_BUDGET.md` 升级为 v0.2，记录 caller 分配 context、禁止动态内存、M2 context size 检查方式；
3. `docs/07_TEST_STRATEGY.md` 增加 M2 API compile test；
4. `docs/08_RISK_REVIEW.md` 保留无 FPU 但允许 float、无参考 IBI、50 Hz 限制等风险。

不要把文档写成长篇论文；保持简洁、可审查。

## 禁止事项

本轮严禁：

1. `malloc` / `calloc` / `realloc`；
2. 第三方 PPG / IBI / HR / HRV 算法库；
3. Python 第三方库；
4. 真实 PPG 峰值检测；
5. 真实 IBI 计算；
6. 输出 `hr_bpm`；
7. 输出 RMSSD / HRV；
8. 引入 CMSIS-DSP；
9. 使用厂商 SDK 特殊数学函数；
10. 大数组上栈；
11. 递归；
12. push；
13. 创建 PR；
14. merge / rebase；
15. 删除远程分支。

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

如果 `grep` 命令因 shell 差异不可用，需报告替代检查结果。

## 通过标准

M2 通过需要满足：

1. `make test` 通过；
2. 公开 header 可被外部 C 测试 include；
3. API 函数存在且可调用；
4. 输出结构不包含 `hr_bpm`、RMSSD 或 HRV 字段；
5. 输入 / 输出字段类型与本任务一致；
6. 状态枚举和 reject reason 已冻结；
7. `confidence` / `signal_quality` 为 0–100 整数；
8. context 由 caller 分配；
9. 未使用动态内存；
10. 未引入外部算法依赖；
11. 未实现真实 IBI 检测；
12. 文档已更新到 v0.2；
13. 没有执行远程 Git 操作。

## 失败时必须报告的信息

如果无法完成，Codex 必须报告：

1. 哪个文件无法创建或修改；
2. 哪条测试命令失败；
3. 失败日志；
4. 是否修改了 API；
5. 是否引入了动态内存；
6. 是否引入了外部依赖；
7. 是否执行了任何 Git 操作；
8. 建议下一步修复点。

## Codex 输出摘要要求

完成后请输出：

```text
Summary
Changed files
Test commands
Test results
Known limitations
是否修改 API：是，本轮按 M2 冻结最小 API
是否修改输出字段：是，本轮按 M2 冻结字段类型，但不新增 hr_bpm
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
