# Test Strategy v0.5 (M8)

## 当前定位

当前无 ECG / 人工标注 / gold standard，因此测试定位为工程行为验证与回归，不做准确性结论。

## 当前主命令

- `make test`
- `make csv-smoke`
- `make resource-report`

## 当前可验证内容

1. API 可编译调用；
2. 逐点输入与状态机语义；
3. strict reject 与 detector reset 行为；
4. 最小 EVENT_READY 工程语义；
5. CSV 闭环输出（`build/output/ibi_events.csv` 与 `build/output/smoke_summary.txt`）；
6. 资源报告输出（`build/output/resource_report.txt`）；
7. 无动态内存、无 HR/RMSSD 输出字段。

## 非目标

- 不评估 MAE/RMSE/matched beats/coverage；
- 不做临床准确性结论。
