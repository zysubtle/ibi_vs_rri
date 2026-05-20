# M4 里程碑启动卡

## 当前里程碑

M4：基础预处理、轻量 SQI 与主通道选择

## 本阶段目的

在 M3 已完成逐点输入校验、timestamp/sample-drop 标记、allow_measure 门控和 PPG 饱和标记的基础上，增加一个不改变 public API 的最小信号质量路径：对每个样本的 4 路 PPG 做基础 raw-range 质量评分，并选择一个主通道。

本阶段仍然保持 no-event 行为，不输出真实 IBI。

## 为什么现在做

M3 已让输入路径稳定可测试；在进入 M5 脉搏候选检测前，需要先让 `selected_channel` 和 `signal_quality` 在 no-event 输出中具备可测试的基础含义。否则 M5 的峰值/周期候选检测会缺少前置质量和通道选择依据。

## 本阶段产出

1. 内部轻量 SQI / channel selection helper；
2. `ppg_ibi_process()` 在 no-event 路径中填充 `selected_channel` 与 `signal_quality`；
3. 保持 `confidence=0`，因为 M4 仍不输出真实 IBI；
4. 新增 M4 host 测试，覆盖主通道选择、低质量、饱和、门控和 no-event；
5. 更新 `docs/07_TEST_STRATEGY.md` 与 `docs/08_RISK_REVIEW.md` 到 M4 视角。

## 本阶段不做

1. 不改 public API；
2. 不扩展 public `ppg_ibi_context_t` 字段；
3. 不实现 IIR/FIR/bandpass/DC removal 等需要历史 buffer 的滤波；
4. 不做真实 SQI 算法、FFT、ACF、峰值检测或真实 IBI 输出；
5. 不读取示例 CSV，不做 CLI / GUI / host 评估工具。

## 验收标准

1. `make test` 通过，且 M2/M3 测试继续通过；
2. public headers 无 diff；
3. `allow_measure=false` 时仍不做 SQI / channel selection，且保持 HOLD/no-event；
4. allow_measure=true 且非饱和样本能得到确定的 `selected_channel` 与 `signal_quality`；
5. 无动态内存、无第三方 PPG/IBI/HR/HRV 算法库、无 `hr_bpm` / RMSSD / HRV 输出。

## 是否需要 Owner 决策

无。M4 明确不改变 public API、不改变输出字段、不改变运动门控策略。若 Codex 认为必须修改 `include/ppg_ibi.h`、`include/ppg_ibi_config.h` 或扩展 public context 才能完成任务，必须暂停并报告为 S0。

## Owner 下一步

将本 ZIP 解压覆盖到仓库根目录，然后把以下一句话交给 Codex：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## docs/10_CODEX_NEXT_TASK.md

本包已提供完整替换版本。
