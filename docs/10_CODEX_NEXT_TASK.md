# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M6：状态机、运动门控与异常恢复语义收敛

## 背景

项目已完成：

1. M2：最小 C API、输入 / 输出结构、状态枚举、reject reason、caller-owned context；
2. M3：逐点输入校验、timestamp gap / sample drop、24-bit PPG 饱和 / 越界标记；
3. M4：最小 raw-range SQI 与确定性主通道选择；
4. M5：最小三点局部峰候选检测、IBI 合法性检查、`EVENT_READY` 输出；
5. M5 Fix 2：strict reject 后 detector reset、跨异常段伪 IBI 风险降低、`EVENT_READY` 字段与候选样本对齐、`TRACK` 状态补齐。

M6 的目标不是增强算法准确性，而是把状态机与异常恢复行为固定成可测试的工程语义，尤其是 `HOLD / REACQUIRE / TRACK`、strict reject、`EVENT_READY` 后 detector history 的行为。

## 本轮任务目标

请实现 M6 状态机、门控与异常恢复语义收敛。

必须完成以下目标：

1. 明确并实现状态转移规则：
   - 初始化后首个可测量、无 strict reject 样本进入 `PPG_IBI_STATE_ACQUIRE`；
   - `allow_measure=false` 立即进入 `PPG_IBI_STATE_HOLD`，不输出 IBI，reset detector；
   - 从 `HOLD` 恢复为 `allow_measure=true` 后进入 `PPG_IBI_STATE_REACQUIRE`；
   - 在 `ACQUIRE` / `REACQUIRE` 中检测到第一个 pulse candidate 时，仅建立 last pulse，不输出 IBI；
   - 合法 IBI event 输出时，`event.state = PPG_IBI_STATE_TRACK`，`ctx->state = PPG_IBI_STATE_TRACK`；
   - 如果处于 `TRACK` 后遇到 strict reject，应退出 `TRACK` 并进入 `REACQUIRE`，除非是 `allow_measure=false`，此时进入 `HOLD`；
   - `IBI_OUT_OF_RANGE` 不输出 event，并应使状态进入 `REACQUIRE` 或保持在可解释的非 `TRACK` 状态；该规则必须写入测试和文档。
2. 统一 strict reject 语义：
   - strict reject 至少包括 `ALLOW_MEASURE_FALSE`、`SATURATED`、`TIMESTAMP_GAP`、`SAMPLE_DROP`、`LOW_SIGNAL_QUALITY`；
   - strict reject 后必须 reset detector history 和 last pulse；
   - strict reject 样本不得写入 detector prev/prev2 history；
   - strict reject 时 `ppg_ibi_process()` 不得返回 `EVENT_READY`。
3. 固化 `EVENT_READY` 后 detector history 行为：
   - 不得因为 `EVENT_READY` 的提前 return 跳过必要的 history 更新，导致下一段检测出现不可解释的 out-of-range 或漏检；
   - 请将 `EVENT_READY` 后当前确认样本如何进入 / 不进入 detector history 的规则实现并测试；
   - 推荐做法：在返回 `EVENT_READY` 前，完成本轮必要的 history 更新，保证后续检测连续且不重复输出同一 pulse。
4. 保持 M5 已有 event 字段一致性：
   - `EVENT_READY` 的 `timestamp_ms`、`sample_index`、`selected_channel`、`signal_quality`、`confidence` 必须来自被确认的 pulse candidate 样本；
   - `ibi_ms` 必须为当前 candidate 与上一有效 pulse candidate 的间隔；
   - `beat_count` 必须只在有效 IBI event 时递增。
5. 做一次最小可读性 / MISRA 风格整理：
   - 将 `src/ppg_ibi.c` 中明显压缩的一行函数、一行多语句改回可审查的多行 C 风格；
   - 不改变行为；
   - 不做大规模架构重构；
   - 不引入新 public API。

## 本轮非目标

本轮明确不做：

1. 不做真实滤波、DC removal、去趋势；
2. 不做滑动窗口 SQI；
3. 不做模板匹配、FFT、ACF、频域逻辑；
4. 不读取 `tests/fixtures/sample_ppg_20000.csv`；
5. 不做真实数据准确性评估；
6. 不计算或输出 HR / HRV / RMSSD；
7. 不引入 CMSIS-DSP；
8. 不引入第三方 PPG / IBI / HR / HRV 算法库；
9. 不做 MCU 交叉编译；
10. 不改变采样率、通道数、IBI 范围常量或运动门控策略。

## 允许修改范围

允许修改：

```text
src/ppg_ibi.c
src/ppg_ibi_internal.h
tests/test_state_machine.c
tests/test_pulse_detector.c
Makefile
docs/04_IO_CONTRACT.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

如果更合理，也可以只新增 `tests/test_state_machine.c`，保留 `tests/test_pulse_detector.c` 作为 M5 测试。

如确有必要，可修改：

```text
include/ppg_ibi.h
```

但仅限于整理或补充 `ppg_ibi_context_t` 内部状态字段。不得修改输入 / 输出结构、函数签名或 enum。

## 禁止修改范围

禁止修改：

1. public function signatures；
2. `ppg_ibi_sample_t`；
3. `ppg_ibi_event_t` 字段；
4. `ppg_ibi_status_t` / `ppg_ibi_state_t` / `ppg_ibi_reject_reason_t` enum 名称或语义；
5. `include/ppg_ibi_config.h`；
6. 采样率、通道数、IBI 范围常量；
7. `allow_measure=false` 立即不输出 IBI 的语义；
8. `AGENTS.md`、`docs/00_PROJECT_BRIEF.md`、`docs/01_DECISION_LOG.md`、`docs/02_MILESTONE_PLAN.md`、`docs/03_ALGORITHM_SCOPE.md`、`docs/05_MCU_ALGORITHM_STRATEGY.md`、`docs/09_CODEX_RUNBOOK.md`、`docs/11_GIT_WORKFLOW.md`，除非只做与 M6 任务引用一致的极小文档修正；
9. Codex 远程 Git 操作。

## MCU 工程约束

必须保持：

1. C99；
2. 无 `malloc/calloc/realloc`；
3. 无动态内存；
4. 无递归；
5. 无不可控大数组上栈；
6. 无第三方算法库；
7. 逐点 `process()`；
8. 固定 4 通道、50 Hz；
9. 允许漏检，优先避免误检；
10. 不宣称准确性或医疗用途。

## 测试要求

请新增或增强测试，至少覆盖以下场景：

1. `INIT -> ACQUIRE`：init 后首个有效样本进入 ACQUIRE；
2. `allow_measure=false -> HOLD`：不输出 event，reset detector；
3. `HOLD -> REACQUIRE`：allow 恢复 true 后进入 REACQUIRE；
4. 首个 pulse candidate 只建立 last pulse，不输出 IBI；
5. 第二个合法 pulse candidate 产生 `EVENT_READY`，状态为 `TRACK`；
6. `TRACK + allow_measure=false`：进入 HOLD，清理 detector 和 last pulse；
7. `TRACK + SATURATED / TIMESTAMP_GAP / SAMPLE_DROP / LOW_SIGNAL_QUALITY`：进入 REACQUIRE 或任务中定义的非 TRACK 状态，不输出 event，reset detector；
8. `IBI_OUT_OF_RANGE`：不输出 event，状态进入 REACQUIRE 或任务中定义的非 TRACK 状态，行为需有断言；
9. `EVENT_READY` 后继续喂入下一组合法 synthetic pulse，不应因 history 不连续产生立即 false IBI，也不应重复输出同一 pulse；
10. 所有 reject 路径下 `ibi_ms=0`，除非返回 `EVENT_READY`。

建议新增：

```text
tests/test_state_machine.c
```

并在 `Makefile` 中加入该测试。现有 M2/M3/M4/M5 测试必须继续通过。

## 文档更新要求

请更新：

```text
docs/04_IO_CONTRACT.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

更新要求：

1. `docs/04_IO_CONTRACT.md` 增加 M6 状态机与 reject 语义说明；
2. `docs/07_TEST_STRATEGY.md` 增加 M6 state machine / recovery test；
3. `docs/08_RISK_REVIEW.md` 增加 M6 对异常恢复、EVENT_READY 后 history 行为、最小 detector 局限的风险处理说明；
4. 不要把文档写成长篇论文；
5. 不要删除长期测试策略，例如示例 CSV smoke test、无 gold standard 限制、Python 标准库规则。

## 测试命令

必须执行并报告结果：

```bash
make test
rg -n "\b(malloc|calloc|realloc)\s*\(" include src tests
rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h
```

期望：

1. `make test` 通过；
2. 动态内存扫描无匹配；
3. HR/RMSSD 禁止字段扫描无匹配；
4. `include/ppg_ibi_config.h` 无差异。

如 `rg` 不可用，可使用等价 `grep -R -E`，但必须说明。

## 通过标准

M6 通过必须满足：

1. 状态机转移规则可由测试证明；
2. strict reject 后 detector history 和 last pulse 均被清理；
3. strict reject 样本不进入 detector history；
4. `EVENT_READY` 字段与 candidate 样本一致，并使 `event.state` / `ctx->state` 为 `TRACK`；
5. `EVENT_READY` 后 history 行为连续、可解释，不产生重复 pulse 或立即 false IBI；
6. `IBI_OUT_OF_RANGE` 不输出 event，并有明确状态行为；
7. M2–M5 既有测试继续通过；
8. 不修改输出字段、函数签名、enum、采样率、通道数、IBI 范围或运动门控策略；
9. 不引入动态内存、外部依赖、第三方算法库或 Python 第三方库；
10. `src/ppg_ibi.c` 至少恢复到可审查的基本多行 C 风格。

## 失败时必须报告

如无法完成，请报告：

1. 哪个通过标准无法满足；
2. 是否需要修改 public API；
3. 是否需要改变状态机语义；
4. 是否需要 Owner 决策；
5. 已执行测试命令与结果；
6. 当前未提交变更状态。

## Codex 输出摘要要求

完成后请报告：

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
commit hash：xxx
```

## Git / PR 限制

请不要 push、不要创建 PR、不要 merge、不要 rebase。只允许本地修改、测试和本地 commit。Owner 自行处理 push 与 PR。
