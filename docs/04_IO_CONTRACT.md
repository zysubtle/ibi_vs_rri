# IO Contract v0.3 (M8)

本文件冻结当前公开 C API、输入输出字段、状态/拒绝原因枚举与工程语义（M8 收敛版）。

## 公开头文件

- `include/ppg_ibi.h`
- `include/ppg_ibi_config.h`

## 核心常量

- `PPG_IBI_CHANNEL_COUNT = 4`
- `PPG_IBI_SAMPLE_RATE_HZ = 50`
- `PPG_IBI_EXPECTED_INTERVAL_MS = 20`
- `PPG_IBI_MIN_IBI_MS = 300`
- `PPG_IBI_MAX_IBI_MS = 2000`

## 输入结构

`ppg_ibi_sample_t`:

- `uint32_t timestamp_ms`（单位 ms）
- `int32_t ppg[4]`（4 路同步 PPG raw，24-bit signed 存放于 int32）
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

约束：当前不输出 `hr_bpm`、HRV、RMSSD。无有效 IBI 时 `ibi_ms=0`。

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

## Public C API

- `void ppg_ibi_config_default(ppg_ibi_config_t *config);`
- `ppg_ibi_status_t ppg_ibi_init(ppg_ibi_context_t *ctx, const ppg_ibi_config_t *config);`
- `ppg_ibi_status_t ppg_ibi_reset(ppg_ibi_context_t *ctx);`
- `ppg_ibi_status_t ppg_ibi_process(ppg_ibi_context_t *ctx, const ppg_ibi_sample_t *sample, ppg_ibi_event_t *event);`
- `const char *ppg_ibi_version(void);`
- `size_t ppg_ibi_context_size(void);`

## 当前工程语义（M8）

1. context 由调用方分配；库内禁止动态内存。
2. `process()` 每次只处理 1 个样本。
3. 当前实现已支持最小三点局部峰 detector 与逐搏事件路径，`PPG_IBI_STATUS_EVENT_READY` 为“工程闭环”语义：
   - 表示该样本处理后形成了可输出的 IBI event；
   - event 字段与被确认的 pulse candidate 对齐；
   - 返回前会推进 history，避免下一拍重复消费同一 candidate。
4. `EVENT_READY` 仅代表工程流程闭环，不代表临床准确性验证完成。
5. 本项目当前仍不输出 `hr_bpm` / HRV / RMSSD。
