# Project Brief v0.1 — MCU PPG-IBI 自研算法

## 1. 项目目标

开发嵌入式 MCU 端 PPG-IBI 自研算法，用于从 4 路同步绿光 PPG 中输出逐搏 IBI，供后续 HRV 分析使用。

本项目当前不负责内部计算 HRV / RMSSD，不作为医疗诊断软件。

## 2. 项目非目标

当前明确不做：

1. 医疗诊断；
2. 算法内部 HRV / RMSSD 计算；
3. 运动中 IBI 输出；
4. 第三方 PPG / IBI / HR / HRV 算法库；
5. GUI；
6. 深度模型或复杂机器学习模型；
7. 基于单个 smoke test 证明总体准确性。

## 3. 输入数据与字段

PPG 输入规格：

| 项目 | 确认值 |
|---|---|
| 通道数 | 4 |
| 光源 | 绿光 |
| 采样率 | 50 Hz |
| 多通道同步 | 严格同步 |
| 分时复用 | 否 |
| 每个样本内容 | 1 个时间点的 4 路 PPG |
| 数据类型 | `int32_t` |
| ADC 位宽 | 24 bit |
| signed / unsigned | signed |
| 有效范围 | -8,388,608 ~ +8,388,607 |
| 可能饱和 | 是 |
| 无效值标志 | 无 |
| 可能丢样 | 是 |

逐点调用：

| 项目 | 确认值 |
|---|---|
| 调用方式 | 逐点 `process()` |
| 每次输入 | 1 个样本，含 4 路 PPG |
| timestamp | 有 |
| timestamp 单位 | ms |
| 固定采样间隔校验 | 需要 |
| sample_counter | 需要 |

示例 CSV 字段：

```text
timestamp_ms, PPG_G1, PPG_G2, PPG_G3, PPG_G4, allow_measure
```

Owner 提供的原始示例文件名为：

```text
示例数据_20000.csv
```

建议入仓库路径：

```text
tests/fixtures/sample_ppg_20000.csv
```

## 4. 输出结果与字段

每次有效 IBI event 建议输出：

1. `timestamp_ms`；
2. `sample_index`；
3. `ibi_ms`；
4. `beat_count` 或 `pulse_index`；
5. `confidence`；
6. `signal_quality`；
7. `selected_channel`；
8. `state`；
9. `reject_reason`；
10. `debug_flags`。

当前不要求输出 `hr_bpm`。

具体枚举值和字段类型在 `docs/04_IO_CONTRACT.md` 中冻结。

## 5. 运动门控策略

| 项目 | 确认值 |
|---|---|
| 是否有 ACC | 无 |
| 运动判断 | 外部传入 `allow_measure` |
| `allow_measure=false` | 立即停止输出 IBI |
| 恢复 `allow_measure=true` | 进入 `REACQUIRE` |
| 运动中 IBI | 不输出 |

## 6. MCU 平台与工程约束

| 项目 | 确认值 |
|---|---|
| 目标 MCU | Apollo3.5 |
| FPU | Owner 当前声明为无 FPU |
| RAM 预算 | 15–20 KB |
| ROM 预算 | 暂不考虑 |
| 单次 process 时间预算 | 暂不考虑 |
| 功耗 | 暂不考虑 |
| 是否允许 float | 是 |
| 是否禁止 malloc/calloc/realloc | 是 |
| MISRA 风格限制 | 需要 |

注意：Owner 声明无 FPU 但允许 float，因此后续需持续记录资源和运行时间风险。

## 7. 自研算法策略

当前策略：

1. 从零自研 MCU 端 PPG-IBI 算法；
2. 禁止第三方 PPG / IBI / HR / HRV 算法库；
3. 允许 C 标准库；
4. 允许 CMSIS-DSP，但不得用作第三方 IBI 算法替代；
5. 不允许厂商 SDK 特殊数学函数；
6. Python 仅可作为 host 测试辅助，且只允许标准库；
7. 核心算法必须保持 MCU 可移植性。

## 8. 状态机默认假设

Owner 接受状态机设计，并允许 Architect 决定异常场景下的状态输出。

候选状态：

```text
INIT / ACQUIRE / TRACK / HOLD / REACQUIRE / INVALID
```

需处理场景包括：

1. 运动门控；
2. 低质量信号；
3. PPG 丢样；
4. timestamp 异常；
5. 通道失效；
6. 信号饱和；
7. 长时间未检测到脉搏。

## 9. 测试与验收标准

当前无 ECG 参考、无人工标注 IBI、无 gold standard。

因此当前阶段只做 smoke test，不做准确性验证。

基础合理性标准：

1. 算法可读取并处理示例 CSV；
2. 输出字段完整；
3. IBI 合理范围：300–2000 ms；
4. 允许漏检，优先避免误检；
5. 运动或低质量时不输出 IBI；
6. 不计算 MAE / RMSE / matched beats / coverage。

## 10. Git / Codex 交付边界

Owner 已确认：

1. Codex 只负责本地编码、运行测试和报告结果；
2. Codex 不 push；
3. Codex 不创建 PR；
4. Codex 不 merge；
5. Owner 自行创建分支、push、创建 PR 和合并 PR；
6. Owner 审查时倾向提供 PR 链接和 Codex 摘要。

## 11. M1 范围

Owner 已选择：

```text
A. 只生成项目文档包
```

因此 M1 不生成源码骨架，不实现算法，不生成测试代码。

## 12. 当前待确认默认假设

1. `beat_count` 与 `pulse_index` 后续在 IO Contract 中择一或定义关系；
2. `confidence` 和 `signal_quality` 的范围后续默认建议为 0–100 或 Q-format，需在 M2 冻结；
3. CMSIS-DSP 是否在早期阶段使用，需由具体任务决定；
4. `allow_measure` 在示例 CSV 中的类型和取值需由实际文件确认；
5. `tests/fixtures/sample_ppg_20000.csv` 需由 Owner 放入仓库。

## 13. 后置问题

1. 是否需要后续 deep research 文档并放入 `docs/research/`；
2. 是否需要 CI；
3. 是否需要固定 C 编译器版本或交叉编译脚本；
4. 是否需要统一 debug log schema；
5. 是否需要外部供应商 API 文档。

## 14. 风险摘要

详细风险见 `docs/08_RISK_REVIEW.md`。

当前主要风险：

1. 无参考 IBI，只能 smoke test；
2. 无 FPU 但允许 float；
3. 50 Hz 对逐搏 IBI 精度存在天然限制；
4. 仅外部 `allow_measure`，无 ACC 内部运动判别；
5. PPG 可饱和、可丢样、无无效值标志。
