# IO Contract v0.3 (M8)

## 公开头文件

- `include/ppg_ibi.h`
- `include/ppg_ibi_config.h`

## 核心常量（冻结）

- `PPG_IBI_CHANNEL_COUNT = 4`
- `PPG_IBI_SAMPLE_RATE_HZ = 50`
- `PPG_IBI_EXPECTED_INTERVAL_MS = 20`
- `PPG_IBI_MIN_IBI_MS = 300`
- `PPG_IBI_MAX_IBI_MS = 2000`

## `ppg_ibi_sample_t`

- `uint32_t timestamp_ms`
- `int32_t ppg[4]`
- `uint8_t allow_measure`

## `ppg_ibi_event_t`

- `uint32_t timestamp_ms`
- `uint32_t sample_index`
- `uint16_t ibi_ms`
- `uint32_t beat_count`
- `uint8_t confidence`
- `uint8_t signal_quality`
- `uint8_t selected_channel`
- `ppg_ibi_state_t state`
- `ppg_ibi_reject_reason_t reject_reason`
- `uint32_t debug_flags`

约束：当前不输出 `hr_bpm` / HRV / RMSSD；无有效 IBI 时 `ibi_ms=0`。

## `ppg_ibi_config_t`

- `uint16_t sample_rate_hz`
- `uint16_t expected_interval_ms`
- `uint16_t min_ibi_ms`
- `uint16_t max_ibi_ms`
- `uint8_t allow_timestamp_strict_check`

## state enum

- `PPG_IBI_STATE_INIT`
- `PPG_IBI_STATE_ACQUIRE`
- `PPG_IBI_STATE_TRACK`
- `PPG_IBI_STATE_HOLD`
- `PPG_IBI_STATE_REACQUIRE`
- `PPG_IBI_STATE_INVALID`

## reject reason enum

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

## status enum

- `PPG_IBI_STATUS_OK`
- `PPG_IBI_STATUS_NO_EVENT`
- `PPG_IBI_STATUS_EVENT_READY`
- `PPG_IBI_STATUS_INVALID_ARGUMENT`
- `PPG_IBI_STATUS_NOT_INITIALIZED`

## public C API

- `void ppg_ibi_config_default(ppg_ibi_config_t *config);`
- `ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx, const ppg_ibi_config_t *config);`
- `ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx);`
- `ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx, const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event);`
- `const char *ppg_ibi_version(void);`
- `size_t ppg_ibi_context_size(void);`

## `EVENT_READY` 当前工程语义（M8）

`PPG_IBI_STATUS_EVENT_READY` 仅表示当前工程实现在约束条件下输出了 1 个 IBI event，适用于工程链路闭环验证。

- 不表示临床准确性已验证。
- 不表示存在 ECG / 人工标注 / gold standard 对齐结论。
