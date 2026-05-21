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

## M5 Fix 2 更新

- 新增最小三点历史 detector 语义：仅在连续 3 点 selected_channel 一致且中点为局部峰时形成 pulse candidate。
- EVENT_READY 字段与被确认 candidate（prev 样本）严格对齐，不改变 event 字段与函数签名。
- 允许扩展 `ppg_ibi_context_t` 内部历史字段，不改变公开 API。

## M6 状态机与 reject 语义补充

- `INIT` 在首个 `allow_measure=true` 且无 strict reject 的样本进入 `ACQUIRE`。
- `allow_measure=false` 立即进入 `HOLD`，不输出 IBI，且清理 detector history 与 last pulse。
- `HOLD` 恢复 `allow_measure=true` 后进入 `REACQUIRE`。
- 在 `ACQUIRE/REACQUIRE` 中检测到第一个 pulse candidate 仅建立 last pulse，不输出 `EVENT_READY`。
- 合法 IBI 事件输出时，`event.state` 与 `ctx->state` 都为 `TRACK`。
- strict reject（`ALLOW_MEASURE_FALSE`/`SATURATED`/`TIMESTAMP_GAP`/`SAMPLE_DROP`/`LOW_SIGNAL_QUALITY`）不返回 `EVENT_READY`，并清理 detector history 与 last pulse；若非 HOLD 场景则进入 `REACQUIRE`。
- `IBI_OUT_OF_RANGE` 不输出 event，状态切回 `REACQUIRE`（非 `TRACK`）。
- `EVENT_READY` 返回前完成本轮 history 更新，避免重复 pulse 或下一拍立即 false IBI。
