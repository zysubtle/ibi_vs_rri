# M8 Acceptance Report

## 1) 当前完成能力

- 保持最小 MCU C API 与逐点处理；
- 保持 strict reject / 状态机恢复语义；
- 保持最小三点局部峰 detector 与 `EVENT_READY` 工程语义；
- 完成 `src/ppg_ibi.c` 与 `tools/ppg_ibi_csv_smoke.c` 可读性整理；
- 新增资源报告命令 `make resource-report`。

## 2) 当前测试命令

- `make test`
- `make csv-smoke`
- `make resource-report`

## 3) 当前 smoke 输出

- `build/output/ibi_events.csv`
- `build/output/smoke_summary.txt`

## 4) 当前不做什么

- 不输出 HR / HRV / RMSSD；
- 不做 ECG 对齐准确性评估；
- 不宣称医疗/临床准确性。

## 5) 当前主要风险

- 缺少 gold standard；
- 50 Hz 分辨率限制；
- 无 FPU 下 float 代价仍待实机复盘；
- detector 仍为最小 synthetic-first 策略。

## 6) 后续建议

在进入真实数据评估/算法增强前，先补齐 gold standard 或人工标注数据，再开展准确性指标评估。
