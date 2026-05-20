# Risk Review v0.1

## 风险分级

- S0：必须 Owner 决策；
- S1：阻塞实现，但 Architect 可给 Codex 修复任务；
- S2：技术风险，记录但不阻塞当前里程碑；
- S3：轻微风格或清理问题。

## 当前风险

| ID | 风险 | 级别 | 当前处理 |
|---|---|---|---|
| R001 | 无 ECG / 人工标注 / gold standard | S2 | 当前只做 smoke test，不评估准确性 |
| R002 | 50 Hz 采样率限制 IBI 精度 | S2 | 后续避免宣称高精度 HRV |
| R003 | Owner 声明无 FPU 但允许 float | S2 | M8 资源复盘 |
| R004 | PPG 可能饱和且无无效值标志 | S2 | 需在检测中识别饱和 |
| R005 | 可能丢样 | S2 | timestamp + sample_counter 检查 |
| R006 | 无 ACC，仅外部 allow_measure | S2 | 不输出运动中 IBI |
| R007 | 允许 CMSIS-DSP 但禁止第三方算法库 | S2 | 任务文件中明确边界 |
| R008 | RAM 15–20 KB | S2 | 禁止动态内存和大栈数组 |
| R009 | 仅单个示例数据 | S2 | 只能作为 smoke test |

## S0 触发条件

以下情况必须暂停 Codex 任务并等待 Owner 决策：

1. 需要改变 API；
2. 需要改变输出字段；
3. 需要引入第三方 PPG / IBI / HR / HRV 算法库；
4. 需要使用 malloc；
5. 需要改变采样率；
6. 需要改变运动门控策略；
7. 需要将项目用于医疗诊断；
8. 需要 Codex push / 创建 PR / merge。
