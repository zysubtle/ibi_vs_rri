# Risk Review v0.5 (M8)

## 核心风险

1. 无 ECG / 人工标注 / gold standard（S2）：当前不能做准确性声明。
2. 50 Hz 采样分辨率限制（S2）：IBI 精度存在天然上限。
3. 无 FPU 但允许 float（S2）：需后续 MCU 实机 profile。
4. detector 仍是 synthetic-first 的最小三点局部峰策略（S2）：真实场景可能漏检/误检。
5. 仅有外部 `allow_measure` 门控，无 ACC 内部运动判别（S2）。

## M8 风险结论

- 当前风险可接受于“工程验证”定位。
- 不可将当前结果外推为临床或泛化准确性。
- 进入真实数据评估/算法增强前，必须准备 gold standard 或人工标注数据。
