# IO Contract v0.3 (M8)

## 冻结范围

公开头文件：
- `include/ppg_ibi.h`
- `include/ppg_ibi_config.h`

M8 未修改 public API、函数签名、枚举、采样率、IBI 范围常量。

## 当前输出语义

`ppg_ibi_event_t` 维持既有字段；当前工程语义支持最小 `EVENT_READY`。

明确：
- 当前不输出 `hr_bpm`；
- 当前不输出 HRV / RMSSD；
- `EVENT_READY` 为工程能力，不代表临床准确性验证完成。
