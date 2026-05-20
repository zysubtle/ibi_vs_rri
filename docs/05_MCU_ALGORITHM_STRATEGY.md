# MCU Algorithm Strategy v0.1

## 总体策略

面向 Apollo3.5 级别 MCU，设计轻量、自研、可测试、可迁移的 PPG-IBI 算法。

当前阶段不追求临床准确性，不做 HRV / RMSSD 计算，不做运动中 IBI 输出。

## 设计原则

1. 逐点输入；
2. 固定 50 Hz；
3. 小状态、小缓存；
4. 禁止动态内存；
5. 避免大数组上栈；
6. 参数集中管理；
7. 状态机可审查；
8. 低质量场景保守拒绝；
9. 允许漏检，避免误检。

## 依赖策略

允许：

1. C 标准库；
2. CMSIS-DSP；
3. Python 标准库作为 host 测试辅助。

禁止：

1. 第三方 PPG / IBI / HR / HRV 算法库；
2. Python 第三方库作为测试依赖；
3. 厂商 SDK 特殊数学函数；
4. malloc / calloc / realloc。

## 推荐算法阶段

M3：输入校验和基础数据路径。

M4：基础预处理、SQI、主通道选择。

M5：脉搏候选检测与 IBI 输出。

M6：状态机、运动门控、异常处理。

## float 使用策略

Owner 允许 float，但声明目标环境无 FPU。

因此：

1. 早期可用 float 快速建立工程算法；
2. 每个 float 模块需可审查；
3. 后续 M8 需评估是否改为定点或限制 float 使用；
4. 禁止无边界使用大规模浮点数组。

## CMSIS-DSP 使用策略

CMSIS-DSP 允许，但必须注意：

1. 不得引入第三方 IBI / HR 算法；
2. 只作为基础 DSP / 数学辅助；
3. 使用前需在具体任务中明确；
4. host 测试需考虑无 CMSIS 环境下的替代或跳过方式。

## 运动门控策略

无 ACC。

外部 `allow_measure` 是唯一运动 / 可测量门控。

规则：

1. `allow_measure=false` 立即停止 IBI 输出；
2. 恢复 true 后进入 `REACQUIRE`；
3. 不输出运动中 IBI；
4. 记录 reject reason。
