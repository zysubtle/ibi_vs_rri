# M6 里程碑启动卡

## 当前里程碑

M6：状态机、运动门控与异常恢复语义收敛

## 本阶段目的

M6 用于把 M2–M5 已经形成的 API、输入校验、SQI、主通道选择、最小脉搏检测与 IBI event 输出行为收敛成可审查、可测试的一套状态机语义。重点是：`allow_measure`、strict reject、`REACQUIRE`、`TRACK`、`EVENT_READY` 后 detector history 的行为一致。

## 为什么现在做

M5 已完成最小候选脉搏检测与 IBI event 输出闭环，并在 Fix 2 中修复了 strict reject 后 detector reset、跨异常段伪 IBI、`EVENT_READY` 字段对齐等问题。进入真实数据 smoke test 或进一步预处理前，需要先把状态机、异常优先级和 detector history 的恢复规则固定下来，避免后续 M7 基于不稳定状态语义测试。

## 本阶段产出

1. 明确并实现 M6 状态机转移规则；
2. 明确 strict reject 优先级和 detector reset 规则；
3. 修正 / 固化 `EVENT_READY` 后当前样本是否进入 detector history 的行为；
4. 新增 M6 host 测试，覆盖状态机、异常恢复、事件后连续检测；
5. 更新 `docs/04_IO_CONTRACT.md`、`docs/07_TEST_STRATEGY.md`、`docs/08_RISK_REVIEW.md` 等文档。

## 本阶段不做

1. 不做真实滤波、DC removal、去趋势；
2. 不做滑动窗口 SQI、模板匹配、FFT、ACF、频域逻辑；
3. 不读取示例 CSV，不做真实数据评估；
4. 不输出 HR / HRV / RMSSD；
5. 不引入 CMSIS-DSP、第三方 PPG / IBI / HR / HRV 算法库或动态内存。

## 验收标准

1. `make test` 通过，并覆盖 M2–M6 相关测试；
2. `ppg_ibi_process()` 的状态转移符合 M6 任务文件定义；
3. strict reject 后 detector history 与 last pulse 均被清理，且 reject 样本不进入 detector history；
4. `EVENT_READY` 时 event 字段与 pulse candidate 一致，同时 `event.state` 与 `ctx->state` 为 `TRACK`；
5. 不修改 public function signatures、`ppg_ibi_sample_t`、`ppg_ibi_event_t`、status/state/reject enum，不引入动态内存或外部依赖。

## 是否需要 Owner 决策

无。M6 不改变已冻结的输入 / 输出字段、公开函数签名、采样率、通道数、IBI 范围、运动门控策略或第三方库策略。若 Codex 认为必须改变这些内容，必须暂停并报告为 S0。

## Owner 下一步

将本 ZIP 解压覆盖到仓库根目录，然后把以下一句话交给 Codex：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## docs/10_CODEX_NEXT_TASK.md

本包已提供完整替换版本。
