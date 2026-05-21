# docs/10_CODEX_NEXT_TASK.md

## 当前 Milestone

M5 Fix 2：修复 M5 最小脉搏候选检测与 IBI event 输出闭环的剩余阻塞问题。

## 背景

当前 M5 修复 PR 已解决部分问题：

1. `ppg_ibi_context_t` 已扩展 detector history；
2. 三点局部峰判定已加入 selected_channel 一致性检查；
3. `EVENT_READY` 的 `timestamp_ms`、`sample_index`、`selected_channel`、`signal_quality`、`confidence` 已尝试对齐到 pulse candidate；
4. 已新增 `tests/test_pulse_detector.c` 并纳入 `make test`。

但审查发现仍存在阻塞问题：

1. detector history 只在 `allow_measure=false` 时部分清理；没有在 saturated、timestamp gap/sample drop、low quality 等 strict reject 路径清理；
2. `allow_measure=false` 仅清理 prev/prev2，但没有清理 `has_last_pulse` / `last_pulse_timestamp_ms`，可能导致恢复后跨异常段计算伪 IBI；
3. `EVENT_READY` 时没有明确将 `event.state` 和 `ctx->state` 更新为 `PPG_IBI_STATE_TRACK`；
4. `tests/test_pulse_detector.c` 的 channel-switch 防伪峰用例没有真正强制 selected_channel 切换，因为多路通道均为 basic-valid 时仍会 tie 到低 index 通道；
5. 当前新 PR 如果替代 PR #4，需要包含完整 M5 文档更新；目前缺少 `docs/04_IO_CONTRACT.md` 与 `docs/06_RESOURCE_BUDGET.md` 的 M5 更新。

## 本轮目标

请在当前 M5 修复 PR 分支上完成最小修复，使 M5 可以通过审查。

必须完成：

1. 新增或恢复统一 detector reset helper，例如：
   - `static void ppg_ibi_reset_detector(ppg_ibi_context_t *ctx);`
2. detector reset 必须清理：
   - `has_prev_sample` / `has_prev2_sample`；
   - prev/prev2 raw；
   - prev/prev2 selected_channel；
   - prev/prev2 signal_quality；
   - prev/prev2 timestamp/sample_index；
   - `has_last_pulse`；
   - `last_pulse_timestamp_ms`；
   - 如 context 中存在 last_pulse_sample_index，也必须清理。
3. strict reject 路径必须触发 detector reset，至少包括：
   - `allow_measure=false`；
   - `PPG_IBI_REJECT_SATURATED`；
   - `PPG_IBI_REJECT_TIMESTAMP_GAP`；
   - `PPG_IBI_REJECT_SAMPLE_DROP`；
   - `PPG_IBI_REJECT_LOW_SIGNAL_QUALITY`。
4. strict reject 样本不得写入 detector prev/prev2 history。
5. `EVENT_READY` 时必须保证事件字段来自被确认的 pulse candidate（prev 样本）：
   - `event.timestamp_ms = prev_timestamp_ms`；
   - `event.sample_index = prev_sample_index`；
   - `event.selected_channel = prev_selected_channel`；
   - `event.signal_quality = prev_signal_quality`；
   - `event.confidence = prev_signal_quality`；
   - `event.ibi_ms` 为当前 candidate 与 last pulse 的间隔；
   - `event.beat_count` 正确递增。
6. `EVENT_READY` 时必须更新状态：
   - `event.state = PPG_IBI_STATE_TRACK`；
   - `ctx->state = PPG_IBI_STATE_TRACK`。
7. 修复 `tests/test_pulse_detector.c`，确保真正覆盖：
   - 三点历史 selected_channel 不一致时不得形成候选峰；
   - strict reject 后不得跨异常段产生 IBI；
   - `allow_measure=false` 后恢复，不得用门控前 last pulse 计算 IBI；
   - saturated / timestamp gap / sample drop / low quality 后恢复，不得跨异常段产生 IBI；
   - `EVENT_READY` 的 `state` 必须是 `PPG_IBI_STATE_TRACK`；
   - `EVENT_READY` 的 timestamp/sample_index/selected_channel/signal_quality/confidence 必须与 candidate 样本一致。
8. 如果该 PR 用于替代 PR #4，必须补齐完整 M5 文档更新：
   - `docs/04_IO_CONTRACT.md`：补充 M5 最小 detector、`EVENT_READY` 语义、允许 context 扩展但不改 function signature / event 字段；
   - `docs/06_RESOURCE_BUDGET.md`：补充 M5 context 增长、无动态内存、常数级逐点计算说明；
   - `docs/07_TEST_STRATEGY.md`：保留 M5 Fix 2 测试说明；
   - `docs/08_RISK_REVIEW.md`：补充 strict reject reset、最小 detector 风险与无 gold standard 限制。

## 本轮非目标

不得实现：

1. 滤波、DC removal、去趋势；
2. 滑动窗口 SQI；
3. 模板匹配、FFT、ACF、频域逻辑；
4. HR / HRV / RMSSD 输出或计算；
5. 示例 CSV 读取或真实数据评估；
6. CMSIS-DSP；
7. 任何第三方 PPG / IBI / HR / HRV 算法库。

## 允许修改范围

允许修改：

1. `include/ppg_ibi.h`
   - 仅允许继续受控扩展或整理 `ppg_ibi_context_t` detector history 字段；
   - 不得修改 `ppg_ibi_sample_t`、`ppg_ibi_event_t`、enum、function signatures。
2. `src/ppg_ibi.c`
   - detector reset、strict reject、event ready 状态与字段一致性修复。
3. `tests/test_pulse_detector.c`
   - 增强 M5 / Fix 2 测试。
4. `Makefile`
   - 如已有 pulse test 目标，原则上不需要大改；只允许必要整理。
5. `docs/04_IO_CONTRACT.md`
6. `docs/06_RESOURCE_BUDGET.md`
7. `docs/07_TEST_STRATEGY.md`
8. `docs/08_RISK_REVIEW.md`

## 禁止修改范围

禁止修改：

1. public function signatures；
2. `ppg_ibi_sample_t`；
3. `ppg_ibi_event_t` 字段；
4. `ppg_ibi_status_t` / `ppg_ibi_state_t` / `ppg_ibi_reject_reason_t` 枚举；
5. `include/ppg_ibi_config.h`，除非仅为无语义影响的注释且必须说明原因；
6. 采样率、通道数、IBI 范围常量；
7. `allow_measure=false` 立即不输出 IBI 的语义；
8. Codex 远程 Git 操作。

## MCU 工程约束

必须保持：

1. C99；
2. 无 malloc/calloc/realloc；
3. 无动态内存；
4. 无递归；
5. 无大数组上栈；
6. 无第三方算法库；
7. 逐点 process；
8. 固定 4 通道、50 Hz。

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

## 通过标准

本轮通过必须满足：

1. 修复跨通道历史混用；
2. strict reject 后 detector history 与 last pulse 均被清理；
3. strict reject 样本不进入 detector history；
4. `EVENT_READY` 字段与 candidate 样本一致；
5. `EVENT_READY` 时 `event.state` 和 `ctx->state` 为 `TRACK`；
6. `tests/test_pulse_detector.c` 覆盖 channel switch、strict reject reset、allow_measure reset、event field consistency；
7. 不修改输出字段、函数签名和 enum；
8. 不引入动态内存或外部依赖；
9. 如果当前 PR 替代 PR #4，则 M5 相关文档必须完整。

## 失败时必须报告

如无法完成，请报告：

1. 哪个通过标准无法满足；
2. 是否需要修改 public API；
3. 是否需要 Owner 决策；
4. 已执行测试命令与结果；
5. 当前未提交变更状态。

## Codex 输出摘要要求

完成后请报告：

1. Summary；
2. Changed files；
3. Test commands；
4. Test results；
5. Known limitations；
6. 是否修改 API；
7. 是否只扩展 `ppg_ibi_context_t`；
8. 是否修改 `ppg_ibi_event_t`；
9. 是否修改 function signatures / enum；
10. 是否引入动态内存；
11. 是否引入外部依赖；
12. 是否执行 Git 操作；
13. commit hash。

## Git / PR 限制

请不要 push、不要创建 PR、不要 merge、不要 rebase。只允许本地修改、测试和本地 commit。Owner 自行处理 push 与 PR。
