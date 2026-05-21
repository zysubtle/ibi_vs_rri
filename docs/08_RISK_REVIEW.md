# Risk Review v0.5 (M8)

## 风险分级

- S0：必须 Owner 决策
- S1：阻塞实现
- S2：技术风险（记录并跟踪）
- S3：轻微问题

## 当前风险清单

- R001（S2）：无 ECG / 人工标注 / gold standard。
- R002（S2）：50Hz 采样率限制逐搏 IBI 分辨率。
- R003（S2）：无 FPU 但允许 float，存在运行时间/功耗风险。
- R004（S2）：依赖外部 `allow_measure`，无 ACC 内建门控。
- R005（S2）：最小三点 detector 仍为 synthetic-first 工程策略。
- R006（S2）：PPG 饱和、丢样、timestamp gap 可能导致漏检/误检。

## S0 触发条件

以下任一发生即 S0：

1. 改变 public API；
2. 改变输出字段语义；
3. 引入第三方 PPG/IBI/HR/HRV 算法库；
4. 引入动态内存；
5. 改变采样率或运动门控策略语义；
6. 声称或用于医疗诊断。

## M8 风险结论

- M8 为收敛阶段：聚焦可维护性、文档权威性、资源可审计性。
- 当前 EVENT_READY 仅是工程事件，不等于准确性或临床有效性。
- 后续若进入真实数据评估，需先补齐 gold standard/标注体系。
