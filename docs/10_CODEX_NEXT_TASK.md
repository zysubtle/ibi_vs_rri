# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M6 Fix 2：补齐完整 M6 行为语义，确保最终 PR 可替代/完成 M6

## 背景

M6 Fix 1 的 PR #8 补充了部分状态机测试，并修复 strict reject 后进入 REACQUIRE 的一部分逻辑。但审查发现：如果 PR #8 作为最终 M6 PR，它仍未完整包含 M6 的行为语义，尤其是：

1. `IBI_OUT_OF_RANGE` 后没有明确让 `ctx->state/event.state` 进入 `REACQUIRE`；
2. `EVENT_READY` 返回前没有更新本轮当前样本到 detector history，可能导致下一拍重新检测同一个 pulse candidate 并触发立即 `IBI_OUT_OF_RANGE`；
3. 当前测试只验证“不立即重复 EVENT_READY”，但没有验证“下一拍不会因同一 pulse 触发 `IBI_OUT_OF_RANGE` / false reject”；
4. 如果 PR #8 替代 PR #7，则需要确保完整 M6 行为和文档均在同一个最终 PR 中。

本轮目标不是重新设计算法，而是做最小修复，使最终 M6 PR 的行为与 M6 要求一致。

## 本轮任务目标

请在当前 M6 修复 PR 分支上继续修复。不要切回 dev 重新开始。

必须完成：

1. 修复 `IBI_OUT_OF_RANGE` 状态语义
   - 当检测到 pulse candidate 但 IBI 不在 `min_ibi_ms` 到 `max_ibi_ms` 范围内时：
     - 返回 `PPG_IBI_STATUS_NO_EVENT`；
     - `event.reject_reason = PPG_IBI_REJECT_IBI_OUT_OF_RANGE`；
     - `ctx->state = PPG_IBI_STATE_REACQUIRE`；
     - `event.state = PPG_IBI_STATE_REACQUIRE`；
     - 不得保持 `TRACK`。

2. 修复 `EVENT_READY` 后 detector history 连续性
   - 在合法 IBI 返回 `PPG_IBI_STATUS_EVENT_READY` 前，应先完成必要的 detector history 更新，或以等价方式保证：
     - 下一拍普通合法样本不会重复使用同一个 candidate pulse；
     - 下一拍普通合法样本不会立即产生 `EVENT_READY`；
     - 下一拍普通合法样本也不应因为同一个 candidate 触发 `IBI_OUT_OF_RANGE` / false reject；
     - 后续下一组合法 synthetic pulse 仍能正常输出新的 `EVENT_READY`。

3. 补强 `tests/test_state_machine.c`
   - 必须新增或调整断言覆盖：
     - `IBI_OUT_OF_RANGE` 后 `event.state == PPG_IBI_STATE_REACQUIRE`，`ctx.state == PPG_IBI_STATE_REACQUIRE`；
     - 一次 `EVENT_READY` 后，紧接着输入一个普通合法样本，应为 `NO_EVENT`，且 `reject_reason` 不应是 `PPG_IBI_REJECT_IBI_OUT_OF_RANGE`；
     - 后续下一组合法 synthetic pulse 到达时才输出新的 `EVENT_READY`；
     - 保留 M6 Fix 1 已覆盖的 `TRACK + allow_measure=false`、`TRACK + TIMESTAMP_GAP`、`TRACK + SAMPLE_DROP`、`TRACK + LOW_SIGNAL_QUALITY`。

4. 如果当前最终 PR 是 PR #8，而不是 PR #7
   - 必须确保该 PR 包含完整 M6 需要的行为语义和文档说明；
   - 不要依赖未合并的 PR #7 中的代码或文档。

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
2. `IBI_OUT_OF_RANGE` 明确进入 `REACQUIRE`，且测试覆盖；
3. `EVENT_READY` 后下一拍不会重复输出同一 pulse，也不会因同一 pulse 立即触发 `IBI_OUT_OF_RANGE`；
4. 后续下一组合法 synthetic pulse 仍能输出新的 `EVENT_READY`；
5. strict reject 后 detector history 和 last pulse 均被清理；
6. strict reject 样本不进入 detector prev/prev2 history；
7. 不修改 public function signatures、event 字段、enum、配置常量；
8. 不引入动态内存、外部依赖或第三方算法库；
9. 文档补充 M6 Fix 2 说明即可，不写成长篇论文。

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
