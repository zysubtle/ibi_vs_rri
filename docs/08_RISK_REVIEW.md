# Risk Review v0.5 (M8)

## 风险分级

- S0：必须 Owner 决策
- S1：阻塞实现
- S2：技术风险，记录但不阻塞当前里程碑
- S3：轻微问题

## 当前风险清单

| ID | 风险 | 级别 | 当前处理 |
|---|---|---|---|
| R001 | 无 ECG / 人工标注 / gold standard | S2 | 当前只做工程闭环，不评估准确性指标 |
| R002 | 50 Hz 采样率限制 IBI 分辨率与精度 | S2 | 保持工程验证定位，不宣称高精度 |
| R003 | 无 FPU 但允许 float | S2 | 当前规模可接受，后续需复盘时延/功耗 |
| R004 | 仅外部 `allow_measure` 门控（无 ACC） | S2 | `allow_measure=false` 进入 HOLD 并停止输出 IBI |
| R005 | detector 为最小三点局部峰策略 | S2 | synthetic-first 可验证，但真实场景仍有漏检/误检风险 |
| R006 | PPG 饱和/越界、timestamp gap、sample drop | S2 | 保持 strict reject 与 REACQUIRE 恢复语义 |
| R007 | RAM 预算 15–20 KB 约束 | S2 | 通过 context size + resource report 持续复盘 |
| R008 | 工程 EVENT_READY 被误解为临床可用 | S2 | 文档明确：仅工程闭环语义，不代表临床准确性 |

## M8 风险结论

1. 当前已具备最小 `EVENT_READY` 工程闭环；
2. 当前不输出 `hr_bpm` / HRV / RMSSD；
3. 当前无 gold standard，不能宣称临床准确性；
4. 算法仍需在真实标注数据上做后续评估与增强。

## S0 触发条件（必须 Owner 决策）

1. 改变 public API / function signature；
2. 改变输出字段语义（含新增 HR/HRV/RMSSD 字段）；
3. 引入第三方 PPG/IBI/HR/HRV 算法库；
4. 引入动态内存（malloc/calloc/realloc）；
5. 改变采样率、通道数、运动门控语义或状态机核心语义；
6. 将项目用途升级为医疗诊断。
