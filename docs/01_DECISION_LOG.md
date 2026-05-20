# Decision Log v0.1

| ID | 决策 | 状态 | 说明 |
|---|---|---|---|
| D001 | 项目目标为 MCU 端 PPG-IBI 自研算法 | 已确认 | 用于后续 HRV 分析 |
| D002 | 不做医疗诊断 | 已确认 | 研究 / 工程验证项目 |
| D003 | 输入为 4 路同步绿光 PPG，50 Hz | 已确认 | 每点含 4 通道 |
| D004 | raw 数据为 `int32_t`，24-bit signed | 已确认 | 范围 -8,388,608 ~ +8,388,607 |
| D005 | 逐点 `process()` 输入 | 已确认 | 每次 1 个样本 |
| D006 | timestamp 单位为 ms | 已确认 | 需校验固定采样间隔 |
| D007 | 需要 sample_counter | 已确认 | 用于内部采样一致性检查 |
| D008 | 无 ACC，由外部 `allow_measure` 门控 | 已确认 | `false` 时立即停止输出 IBI |
| D009 | 恢复 `allow_measure=true` 后进入 `REACQUIRE` | 已确认 | 不保留运动中 IBI |
| D010 | 目标 MCU 为 Apollo3.5 | 已确认 | Owner 当前声明无 FPU |
| D011 | RAM 预算 15–20 KB | 已确认 | ROM 暂不考虑 |
| D012 | 允许 float | 已确认 | 需记录无 FPU 风险 |
| D013 | 禁止 malloc / calloc / realloc | 已确认 | MCU 约束 |
| D014 | 需要 MISRA 风格限制 | 已确认 | 风格与安全性约束 |
| D015 | 禁止第三方 PPG / IBI / HR / HRV 算法库 | 已确认 | 算法自研 |
| D016 | 允许 C 标准库 | 已确认 | 基础函数可用 |
| D017 | 允许 CMSIS-DSP | 已确认 | 但不得作为第三方 IBI 算法替代 |
| D018 | 不允许厂商 SDK 特殊数学函数 | 已确认 | 降低平台绑定 |
| D019 | Python host 测试只允许标准库 | 已确认 | 不替代 MCU 算法 |
| D020 | 无 ECG / 人工标注 / gold standard | 已确认 | 当前只做 smoke test |
| D021 | IBI 合理范围 300–2000 ms | 已确认 | 允许漏检，避免误检 |
| D022 | M1 只生成项目文档包 | 已确认 | 不生成源码骨架 |
| D023 | Codex 不 push、不创建 PR、不 merge | 已确认 | Owner 自行管理 Git / PR |

## S0 决策规则

以下事项必须由 Owner 决策，Architect / Codex 不得擅自改变：

1. 改变公开 API；
2. 改变输入 / 输出字段；
3. 改变核心算法策略；
4. 引入第三方 PPG / IBI / HR / HRV 算法库；
5. 改变是否允许 float；
6. 改变是否允许 CMSIS-DSP；
7. 改变 malloc 禁止规则；
8. 改变 RAM 预算；
9. 改变采样率；
10. 改变运动门控策略；
11. 要求 Codex push、创建 PR 或 merge。
