# IO Contract v0.2

本文件冻结 M2 阶段最小 C API、输入输出字段、状态枚举与 reject reason。

## 公开头文件

- `include/ppg_ibi.h`
- `include/ppg_ibi_config.h`

## 常量

- `PPG_IBI_CHANNEL_COUNT = 4`
- `PPG_IBI_SAMPLE_RATE_HZ = 50`
- `PPG_IBI_EXPECTED_INTERVAL_MS = 20`
- `PPG_IBI_MIN_IBI_MS = 300`
- `PPG_IBI_MAX_IBI_MS = 2000`

## 输入结构

`ppg_ibi_sample_t`:

- `uint32_t timestamp_ms`（ms）
- `int32_t ppg[4]`（4 路同步 PPG raw, 24-bit signed 存放于 int32）
- `uint8_t allow_measure`（0=false，非 0=true）

## 输出结构

`ppg_ibi_event_t`:

- `uint32_t timestamp_ms`
- `uint32_t sample_index`
- `uint16_t ibi_ms`
- `uint32_t beat_count`
- `uint8_t confidence`（0–100）
- `uint8_t signal_quality`（0–100）
- `uint8_t selected_channel`（0–3；255=invalid）
- `ppg_ibi_state_t state`
- `ppg_ibi_reject_reason_t reject_reason`
- `uint32_t debug_flags`

约束：当前不输出 `hr_bpm`、RMSSD、HRV。无有效 IBI 时 `ibi_ms=0`。

## 状态枚举

- `PPG_IBI_STATE_INIT`
- `PPG_IBI_STATE_ACQUIRE`
- `PPG_IBI_STATE_TRACK`
- `PPG_IBI_STATE_HOLD`
- `PPG_IBI_STATE_REACQUIRE`
- `PPG_IBI_STATE_INVALID`

## reject reason 枚举

- `PPG_IBI_REJECT_NONE`
- `PPG_IBI_REJECT_ALLOW_MEASURE_FALSE`
- `PPG_IBI_REJECT_LOW_SIGNAL_QUALITY`
- `PPG_IBI_REJECT_SATURATED`
- `PPG_IBI_REJECT_TIMESTAMP_GAP`
- `PPG_IBI_REJECT_SAMPLE_DROP`
- `PPG_IBI_REJECT_CHANNEL_INVALID`
- `PPG_IBI_REJECT_IBI_OUT_OF_RANGE`
- `PPG_IBI_REJECT_NO_STABLE_PULSE`
- `PPG_IBI_REJECT_INTERNAL_INVALID`

## status 返回值

- `PPG_IBI_STATUS_OK`
- `PPG_IBI_STATUS_NO_EVENT`
- `PPG_IBI_STATUS_EVENT_READY`
- `PPG_IBI_STATUS_INVALID_ARGUMENT`
- `PPG_IBI_STATUS_NOT_INITIALIZED`

## 配置结构

`ppg_ibi_config_t`:

- `uint16_t sample_rate_hz`（默认 50）
- `uint16_t expected_interval_ms`（默认 20）
- `uint16_t min_ibi_ms`（默认 300）
- `uint16_t max_ibi_ms`（默认 2000）
- `uint8_t allow_timestamp_strict_check`（默认 1）

## 最小 C API

- `void ppg_ibi_config_default(ppg_ibi_config_t *config);`
- `ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx, const ppg_ibi_config_t *config);`
- `ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx);`
- `ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx, const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event);`
- `const char *ppg_ibi_version(void);`
- `size_t ppg_ibi_context_size(void);`

语义冻结：

1. context 由调用方分配；
2. 库内禁止动态内存；
3. `process()` 每次处理 1 个样本；
4. M2 不输出真实 IBI，仅返回 `NO_EVENT` 占位行为。


## M5 更新（最小脉搏候选与 IBI 事件）

- 允许在 `ppg_ibi_context_t` 内扩展 detector 历史字段（prev/prev2 raw、timestamp、sample_index、last_pulse 等），不改变 public function signatures。
- 在门控和质量都通过时，基于 selected channel 执行三点局部峰候选；第二个候选起计算 IBI。
- 当 IBI 落在 300–2000 ms，`ppg_ibi_process()` 返回 `PPG_IBI_STATUS_EVENT_READY`，并填充 event 的 timestamp/sample_index/ibi_ms/beat_count/confidence/signal_quality/selected_channel/state/reject/debug。
- 低质量、饱和、timestamp 异常、sample drop、allow_measure=false 时不输出 IBI event。
