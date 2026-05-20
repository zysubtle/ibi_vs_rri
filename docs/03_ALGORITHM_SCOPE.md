# Algorithm Scope v0.1

## 核心范围

本项目算法核心是：

```text
4 路同步绿光 PPG → 自研 MCU 端处理 → 有效逐搏 IBI event
```

## 当前输入

1. 4 路 PPG；
2. 50 Hz；
3. `int32_t` 24-bit signed raw；
4. `timestamp_ms`；
5. `allow_measure`。

## 当前输出

有效 IBI event，字段由 `docs/04_IO_CONTRACT.md` 定义。

## 推荐模块划分

后续源码可按以下模块拆分，但 M1 不生成源码：

1. 输入校验与采样计数；
2. 饱和 / 异常检测；
3. 轻量预处理；
4. SQI；
5. 主通道选择；
6. 脉搏候选检测；
7. IBI 合法性检查；
8. 状态机；
9. 输出事件；
10. host smoke test。

## 明确不做

1. 不做 HRV / RMSSD 内部计算；
2. 不输出运动中 IBI；
3. 不引入第三方 PPG / IBI / HR / HRV 算法库；
4. 不做医疗诊断；
5. 不做复杂模型；
6. 不做 GUI。

## 低质量策略

原则：宁可漏检，不应误检。

当出现以下情况时，应降低 confidence 或不输出 IBI：

1. `allow_measure=false`；
2. 饱和；
3. timestamp 异常；
4. 丢样；
5. 通道质量差；
6. 长时间无法稳定检测；
7. IBI 超出 300–2000 ms。
