# PR #12 复审追加修复要求 — M6 Fix 5

PR #12 仍未通过。当前阻塞点是：IBI_OUT_OF_RANGE 分支没有完整保留 M6 语义。

请在当前 M6 修复 PR 分支继续修复，并且不得回退 PR #12 已完成的 strict reject -> REACQUIRE 和 EVENT_READY 后 history 连续性修复。

## 本轮只修复以下内容

1. 修复 `ppg_ibi_process()` 中 `PPG_IBI_REJECT_IBI_OUT_OF_RANGE` 分支：
   - 返回 `PPG_IBI_STATUS_NO_EVENT`；
   - `event.reject_reason = PPG_IBI_REJECT_IBI_OUT_OF_RANGE`；
   - `ctx->state = PPG_IBI_STATE_REACQUIRE`；
   - `event->state = PPG_IBI_STATE_REACQUIRE`；
   - 清理 detector history；
   - 清理 last pulse；
   - out-of-range 样本不得继续污染 detector prev/prev2 history。

2. 在 `tests/test_state_machine.c` 中新增或恢复测试：
   - 构造一个过短 IBI 或过长 IBI；
   - 断言返回 `PPG_IBI_STATUS_NO_EVENT`；
   - 断言 `event.reject_reason == PPG_IBI_REJECT_IBI_OUT_OF_RANGE`；
   - 断言 `event.state == PPG_IBI_STATE_REACQUIRE`；
   - 断言 `ctx.state == PPG_IBI_STATE_REACQUIRE`；
   - 断言 detector history 和 last pulse 已清理。

3. 保留 PR #12 已修复内容：
   - `SATURATED / TIMESTAMP_GAP / SAMPLE_DROP / LOW_SIGNAL_QUALITY -> REACQUIRE`；
   - strict reject 后 reset detector history / last pulse；
   - `EVENT_READY` 返回前推进 detector history；
   - `EVENT_READY` 后下一拍普通合法样本 `NO_EVENT`，且不是 `IBI_OUT_OF_RANGE`；
   - 后续下一组合法 synthetic pulse 才能再次 `EVENT_READY`。

## 禁止事项

不得修改 public function signatures、`ppg_ibi_event_t`、status/state/reject enum、采样率、通道数、IBI 范围常量、`include/ppg_ibi_config.h`。

不得引入 malloc/calloc/realloc、外部依赖、第三方 PPG/IBI/HR/HRV 算法库。

## 必须运行并报告

make test
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests
rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h


# PR #11 复审追加修复要求 — M6 Fix 4

PR #11 仍未通过。当前阻塞点是：新增状态机测试中的“后续 synthetic pulse 触发第二次 EVENT_READY”场景未通过，且测试本身存在时间戳回退问题，无法有效验证 EVENT_READY 后 history 连续性。

请在当前 M6 修复 PR 分支上继续修复，并且不得回退既有 M6 Fix 3 行为。

## 本轮只修复以下内容

1. 修复 `tests/test_state_machine.c` 中 EVENT_READY 后连续性测试的 synthetic 时间轴：
   - 第一次合法 `EVENT_READY` 后，下一拍普通合法样本必须使用递增 timestamp；
   - 不得出现 timestamp 回退，例如在 6360 ms 后又输入 6280 ms；
   - 该下一拍样本应返回 `PPG_IBI_STATUS_NO_EVENT`；
   - 断言 `reject_reason != PPG_IBI_REJECT_IBI_OUT_OF_RANGE`。

2. 保留并验证 EVENT_READY 后 history 连续性：
   - 一次合法 `EVENT_READY` 后，不得在下一拍重复消费同一个 pulse candidate；
   - 后续只有当下一组新的合法 synthetic pulse 到达时，才允许再次返回 `PPG_IBI_STATUS_EVENT_READY`。

3. 如果修正测试后仍无法通过，才允许对 `src/ppg_ibi.c` 做最小行为修复：
   - 修复范围仅限 detector history 推进 / EVENT_READY 后连续性；
   - 不得改变 public API、`ppg_ibi_event_t`、function signatures、status/state/reject enum、采样率、通道数、IBI 范围常量。

4. 必须保留既有 M6 语义：
   - `allow_measure=false -> HOLD`，并 reset detector history / last pulse；
   - `SATURATED / TIMESTAMP_GAP / SAMPLE_DROP / LOW_SIGNAL_QUALITY -> REACQUIRE`，并 reset detector history / last pulse；
   - `IBI_OUT_OF_RANGE -> REACQUIRE`；
   - `EVENT_READY` 时 `event.state` 与 `ctx->state` 均为 `TRACK`；
   - `EVENT_READY` 字段对齐到被确认的 pulse candidate 样本。

## 禁止事项

1. 不得修改 public function signatures；
2. 不得修改 `ppg_ibi_event_t` 字段；
3. 不得修改 status/state/reject enum；
4. 不得修改 `include/ppg_ibi_config.h`；
5. 不得引入 `malloc/calloc/realloc`；
6. 不得引入外部依赖或第三方 PPG/IBI/HR/HRV 算法库；
7. 不得实现 HR/HRV/RMSSD 输出；
8. 不得 push、创建 PR 或 merge。

## 必须运行并报告

```bash
make test
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests
rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h
```

## 通过标准

1. `make test` 必须通过；
2. EVENT_READY 后的下一拍普通合法样本必须 `NO_EVENT` 且不是 `IBI_OUT_OF_RANGE`；
3. 后续新的合法 synthetic pulse group 必须能再次触发 `EVENT_READY`；
4. strict reject 与 `IBI_OUT_OF_RANGE` 状态语义不得回退；
5. 禁止项扫描无命中；
6. `include/ppg_ibi_config.h` 无差异。


# PR #10 复审追加修复要求

PR #10 仍未通过，原因是 EVENT_READY 后 history 连续性修复未保留。

请在当前 PR #10 分支继续修复，并且不得回退现有 M6 Fix 3 的其他要求。

本轮只补以下内容：

1. 在 `ppg_ibi_process()` 的 `EVENT_READY` 返回前推进 detector history，或实现等价机制，确保不会在下一拍重复消费同一个 pulse candidate。
2. 在 `tests/test_state_machine.c` 中新增测试：
   - 先构造一次合法 `EVENT_READY`；
   - 紧接着输入一个普通合法样本；
   - 断言返回 `PPG_IBI_STATUS_NO_EVENT`；
   - 断言 `reject_reason != PPG_IBI_REJECT_IBI_OUT_OF_RANGE`；
   - 再输入下一组合法 synthetic pulse；
   - 断言只有下一组合法 pulse 才产生新的 `EVENT_READY`。
3. 保留 `IBI_OUT_OF_RANGE -> REACQUIRE` 行为。
4. 保留 strict reject 行为：
   - `SATURATED / TIMESTAMP_GAP / SAMPLE_DROP / LOW_SIGNAL_QUALITY -> REACQUIRE`
   - `allow_measure=false -> HOLD`
   - strict reject 后清理 detector history 和 last pulse
5. 不修改 public function signatures、`ppg_ibi_event_t`、enum、采样率、通道数、IBI 范围常量。
6. 不引入 `malloc/calloc/realloc`、外部依赖或第三方算法库。

必须运行并报告：

```bash
make test
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests
rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h


# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M6 Fix 3：补齐最终 M6 PR 的完整状态机语义与测试覆盖

## 背景

M6 Fix 2 的 PR #9 已修复部分问题：

1. `IBI_OUT_OF_RANGE` 后进入 `REACQUIRE`；
2. `EVENT_READY` 返回前推进 detector history；
3. 新增了部分 `EVENT_READY` 后连续性测试。

但审查发现：如果 PR #9 作为最终 M6 PR，它仍未完整包含 M6 的状态机语义，尤其是：

1. strict reject 路径没有统一让运行状态进入 `REACQUIRE`；
2. strict reject 相关测试只检查了部分 `reject_reason`，未检查状态迁移和 detector/last-pulse reset；
3. 当前 PR #9 不能依赖未合并的 PR #7 / PR #8 中的行为；
4. 最终 M6 PR 必须自包含完整 M6 行为与测试。

本轮目标不是扩展算法能力，而是补齐最终 M6 PR 的状态机闭环。

## 本轮任务目标

请在当前 M6 修复 PR 分支上继续修复。不要切回 dev 重新开始。

必须完成：

### 1. strict reject 状态语义补齐

在 `ppg_ibi_process()` 中，当 `allow_measure=true` 且发生以下 strict reject 时：

```text
PPG_IBI_REJECT_SATURATED
PPG_IBI_REJECT_TIMESTAMP_GAP
PPG_IBI_REJECT_SAMPLE_DROP
PPG_IBI_REJECT_LOW_SIGNAL_QUALITY
```

必须：

```text
return PPG_IBI_STATUS_NO_EVENT
event.reject_reason = 对应 reject reason
ctx->state = PPG_IBI_STATE_REACQUIRE
event->state = PPG_IBI_STATE_REACQUIRE
清理 detector history
清理 last pulse
strict reject 样本不得进入 detector prev/prev2 history
```

`allow_measure=false` 的语义保持：

```text
return PPG_IBI_STATUS_NO_EVENT
event.reject_reason = PPG_IBI_REJECT_ALLOW_MEASURE_FALSE
ctx->state = PPG_IBI_STATE_HOLD
event->state = PPG_IBI_STATE_HOLD
清理 detector history
清理 last pulse
```

### 2. 保留 M6 Fix 2 已修复语义

不得回退以下行为：

```text
IBI_OUT_OF_RANGE 后：
  return PPG_IBI_STATUS_NO_EVENT
  event.reject_reason = PPG_IBI_REJECT_IBI_OUT_OF_RANGE
  ctx->state = PPG_IBI_STATE_REACQUIRE
  event->state = PPG_IBI_STATE_REACQUIRE

EVENT_READY 后：
  返回前推进 detector history 或等价保证 history 连续性
  下一拍普通合法样本为 NO_EVENT
  下一拍普通合法样本不得因同一 pulse 触发 IBI_OUT_OF_RANGE
  后续下一组合法 synthetic pulse 才能输出新的 EVENT_READY
```

### 3. 补强 `tests/test_state_machine.c`

必须新增或调整断言覆盖：

1. `TRACK + allow_measure=false`
   - `event.state == PPG_IBI_STATE_HOLD`
   - `ctx.state == PPG_IBI_STATE_HOLD`
   - detector history 和 last pulse 均被清理

2. `TRACK + SATURATED`
   - `event.state == PPG_IBI_STATE_REACQUIRE`
   - `ctx.state == PPG_IBI_STATE_REACQUIRE`
   - `event.reject_reason == PPG_IBI_REJECT_SATURATED`
   - detector history 和 last pulse 均被清理

3. `TRACK + TIMESTAMP_GAP`
   - `event.state == PPG_IBI_STATE_REACQUIRE`
   - `ctx.state == PPG_IBI_STATE_REACQUIRE`
   - `event.reject_reason == PPG_IBI_REJECT_TIMESTAMP_GAP`
   - detector history 和 last pulse 均被清理

4. `TRACK + SAMPLE_DROP`
   - `event.state == PPG_IBI_STATE_REACQUIRE`
   - `ctx.state == PPG_IBI_STATE_REACQUIRE`
   - `event.reject_reason == PPG_IBI_REJECT_SAMPLE_DROP`
   - detector history 和 last pulse 均被清理

5. `TRACK + LOW_SIGNAL_QUALITY`
   - `event.state == PPG_IBI_STATE_REACQUIRE`
   - `ctx.state == PPG_IBI_STATE_REACQUIRE`
   - `event.reject_reason == PPG_IBI_REJECT_LOW_SIGNAL_QUALITY`
   - detector history 和 last pulse 均被清理

6. 保留 M6 Fix 2 测试：
   - `IBI_OUT_OF_RANGE -> REACQUIRE`
   - `EVENT_READY` 后下一拍普通样本 `NO_EVENT`
   - 下一拍普通样本不得是 `IBI_OUT_OF_RANGE`
   - 后续下一组合法 synthetic pulse 才输出新的 `EVENT_READY`

### 4. 保证最终 PR 自包含

如果当前 PR #9 替代 PR #7 / PR #8，必须确保 PR #9 自己包含完整 M6 行为、测试和必要文档说明。

不得依赖未合并 PR #7 / PR #8 中的代码、测试或文档。

## 允许修改范围

允许修改：

```text
src/ppg_ibi.c
tests/test_state_machine.c
tests/test_pulse_detector.c
Makefile
docs/04_IO_CONTRACT.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

如确需补充资源说明，可修改：

```text
docs/06_RESOURCE_BUDGET.md
```

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

除非绝对必要，不要再扩展 `ppg_ibi_context_t`。

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
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests
rg -n "hr_bpm|rmssd|RMSSD" include src tests
git diff -- include/ppg_ibi_config.h
```

如 `rg` 不可用，可用等价 `grep -R -E`，但必须说明。

## 通过标准

本轮通过标准：

1. `make test` 通过；
2. strict reject 状态语义完整：
   - allow false -> HOLD；
   - SATURATED / TIMESTAMP_GAP / SAMPLE_DROP / LOW_SIGNAL_QUALITY -> REACQUIRE；
3. strict reject 后 detector history 和 last pulse 均被清理；
4. strict reject 样本不进入 detector prev/prev2 history；
5. `IBI_OUT_OF_RANGE` 明确进入 `REACQUIRE`，且测试覆盖；
6. `EVENT_READY` 后下一拍不会重复输出同一 pulse，也不会因同一 pulse 立即触发 `IBI_OUT_OF_RANGE`；
7. 后续下一组合法 synthetic pulse 仍能输出新的 `EVENT_READY`；
8. 不修改 public function signatures、event 字段、enum、配置常量；
9. 不引入动态内存、外部依赖或第三方算法库；
10. 文档补充 M6 Fix 3 说明即可，不写成长篇论文。

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
