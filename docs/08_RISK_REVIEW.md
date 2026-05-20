# Risk Review v0.4

## 风险分级

- S0：必须 Owner 决策
- S1：阻塞实现
- S2：技术风险，记录但不阻塞当前里程碑
- S3：轻微问题

## 当前风险清单

| ID | 风险 | 级别 | 当前处理 |
|---|---|---|---|
| R001 | 无 ECG / 人工标注 / gold standard | S2 | 当前只做 API/Smoke，不评估准确性 |
| R002 | 50 Hz 采样率限制 IBI 精度 | S2 | M4 仍不输出真实 IBI，不宣称高精度 |
| R003 | 无 FPU 但允许 float | S2 | M4 继续避免浮点重计算，后续仍需复盘运行时间/功耗 |
| R004 | PPG 可饱和且无 invalid 标志 | S2 | M4 对边界/越界继续 SATURATED reject，并补充基础通道质量评分 |
| R005 | 可能丢样 / timestamp gap | S2 | M3 既有 gap 与 sample drop 区分标记延续至 M4 |
| R006 | 无 ACC，仅外部 allow_measure | S2 | `allow_measure=false` 立即停止 IBI 输出并进入 HOLD |
| R007 | RAM 15–20 KB 预算 | S2 | M4 采用逐样本、无历史缓存 SQI，不引入动态内存 |
| R008 | 当前无真实 IBI 检测 | S2 | 里程碑允许，M4 仅补充最小 SQI 与主通道选择 |

## M4 风险处理状态

1. 基础 SQI：仅基于 raw range 和 near-saturation 阈值评分（0/20/80），不使用滤波、滑窗或频域方法；
2. 主通道选择：每样本在 4 路中选最高质量通道，同分取最小 index，保证确定性；
3. 低质量处理：在无更高优先级 reject 时，`signal_quality<threshold` 触发 `LOW_SIGNAL_QUALITY`；
4. 饱和优先：任一路达到 24-bit 边界或越界仍优先触发 `SATURATED`；
5. 算法边界：当前仍不输出真实 IBI，不返回 `EVENT_READY`。

## S0 触发条件（保持不变）

1. 改变公开 API 或输出字段语义；
2. 引入第三方 PPG/IBI/HR/HRV 算法库；
3. 引入动态内存；
4. 改变采样率或运动门控策略；
5. 用于医疗诊断。
