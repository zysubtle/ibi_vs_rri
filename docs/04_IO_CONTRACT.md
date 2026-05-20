# IO Contract v0.1

本文件定义 M1 阶段的 IO Contract 草案。M2 需要进一步冻结字段类型、枚举值和 API 形态。

## 输入样本

建议输入字段：

| 字段 | 类型 | 说明 |
|---|---|---|
| `timestamp_ms` | `uint32_t` 或 `uint64_t` 待 M2 决策 | 样本时间戳，单位 ms |
| `ppg[4]` | `int32_t[4]` | 4 路同步 PPG raw |
| `allow_measure` | `bool` 或 `uint8_t` 待 M2 决策 | 外部可测量门控 |

约束：

1. 采样率固定为 50 Hz；
2. 期望相邻样本间隔为 20 ms；
3. 每次 `process()` 输入 1 个样本；
4. 每个样本包含 4 个同步通道；
5. raw 为 24-bit signed，存放于 `int32_t`。

## 输出事件

建议 IBI event 字段：

| 字段 | 建议类型 | 说明 |
|---|---|---|
| `timestamp_ms` | 待 M2 冻结 | IBI 对应时间戳 |
| `sample_index` | 待 M2 冻结 | 内部样本序号 |
| `ibi_ms` | 待 M2 冻结 | IBI，单位 ms |
| `beat_count` | 待 M2 冻结 | 有效 beat 计数 |
| `confidence` | 待 M2 冻结 | 置信度，范围待定 |
| `signal_quality` | 待 M2 冻结 | 信号质量，范围待定 |
| `selected_channel` | 待 M2 冻结 | 当前选择通道，0–3 或 INVALID |
| `state` | enum | 当前状态 |
| `reject_reason` | enum | 拒绝原因 |
| `debug_flags` | bitmask | 调试标志 |

当前不输出：

```text
hr_bpm
rmssd
hrv metrics
```

## 状态枚举草案

```text
INIT
ACQUIRE
TRACK
HOLD
REACQUIRE
INVALID
```

M2 需冻结实际 C enum 命名。

## reject_reason 草案

建议包含：

```text
NONE
ALLOW_MEASURE_FALSE
LOW_SIGNAL_QUALITY
SATURATED
TIMESTAMP_GAP
SAMPLE_DROP
CHANNEL_INVALID
IBI_OUT_OF_RANGE
NO_STABLE_PULSE
INTERNAL_INVALID
```

M2 需冻结实际枚举和优先级。

## 输出策略

1. 只有检测到有效 IBI 时输出 event；
2. `allow_measure=false` 时立即停止输出 IBI；
3. 恢复 `allow_measure=true` 后进入 `REACQUIRE`；
4. IBI 合理范围为 300–2000 ms；
5. 允许漏检，避免误检。

## 待 M2 决策

1. `timestamp_ms` 类型；
2. `sample_index` 类型；
3. `ibi_ms` 类型；
4. `confidence` 范围；
5. `signal_quality` 范围；
6. `beat_count` 与 `pulse_index` 的关系；
7. C API 函数签名；
8. context 分配方式；
9. 输出 event 的返回方式。
