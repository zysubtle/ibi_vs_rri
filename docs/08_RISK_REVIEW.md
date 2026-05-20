# Risk Review v0.2

## 风险分级

- S0：必须 Owner 决策
- S1：阻塞实现
- S2：技术风险，记录但不阻塞当前里程碑
- S3：轻微问题

## 当前风险清单

| ID | 风险 | 级别 | 当前处理 |
|---|---|---|---|
| R001 | 无 ECG / 人工标注 / gold standard | S2 | 当前只做 API/Smoke，不评估准确性 |
| R002 | 50 Hz 采样率限制 IBI 精度 | S2 | M2 仅冻结接口，不宣称高精度 |
| R003 | 无 FPU 但允许 float | S2 | 记录运行时间/功耗风险，后续复盘 |
| R004 | PPG 可饱和且无 invalid 标志 | S2 | 已冻结 reject reason，后续实现识别逻辑 |
| R005 | 可能丢样 / timestamp gap | S2 | M2 占位实现包含 timestamp gap reject |
| R006 | 无 ACC，仅外部 allow_measure | S2 | `allow_measure=false` 立即停止 IBI 输出 |
| R007 | RAM 15–20 KB 预算 | S2 | caller 分配 context，禁动态内存 |
| R008 | 当前无真实 IBI 检测 | S2 | 里程碑允许，后续 M3+ 逐步实现 |

## S0 触发条件（保持不变）

1. 改变公开 API 或输出字段语义；
2. 引入第三方 PPG/IBI/HR/HRV 算法库；
3. 引入动态内存；
4. 改变采样率或运动门控策略；
5. 用于医疗诊断。
