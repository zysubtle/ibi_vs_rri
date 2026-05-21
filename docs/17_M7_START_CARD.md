# M7 里程碑启动卡

## 当前里程碑

M7：host 端 CSV smoke test、IBI event 导出与基础统计检查

## 本阶段目的

在 M2–M6 已完成 MCU C API、输入校验、SQI / 主通道选择、最小 IBI event 输出和状态机收敛之后，M7 通过 host 端 CSV smoke test 验证算法可以在真实仓库 fixture 上逐点运行，并导出 IBI event 与基础统计结果。

## 为什么现在做

当前算法已有最小 `EVENT_READY` 工程闭环，但此前测试主要是 synthetic 单元测试。进入资源复盘或算法增强前，必须先确认：

1. 示例 CSV 能被仓库工具稳定读取；
2. 字段、timestamp、allow_measure 与 IO Contract 一致；
3. C 算法能逐点跑完整个 fixture；
4. 输出 event CSV 和 summary；
5. 不宣称准确性，仅做 smoke / 工程合理性验证。

## 本阶段产出

1. host 端 CSV smoke 工具，例如 `tools/ppg_ibi_csv_smoke.c`；
2. Makefile 中新增 CSV smoke target；
3. 输出 `build/output/ibi_events.csv`；
4. 输出 `build/output/smoke_summary.txt`；
5. 更新测试策略 / 风险文档，说明无 gold standard 下仅做 smoke test。

## 本阶段不做

1. 不做 ECG / gold standard 对齐；
2. 不计算 MAE / RMSE / matched beats / coverage；
3. 不宣称真实准确率；
4. 不做 HRV / RMSSD 输出；
5. 不引入第三方算法库或 Python 第三方依赖；
6. 不做 GUI；
7. 不做滤波、模板匹配、FFT、ACF 等复杂算法增强。

## 验收标准

1. `make test` 通过；
2. 如果 `tests/fixtures/sample_ppg_20000.csv` 存在，则 `make csv-smoke` 必须通过；
3. CSV smoke 工具能读取 `timestamp_ms,PPG_G1,PPG_G2,PPG_G3,PPG_G4,allow_measure`；
4. 输出 event CSV 至 `build/output/ibi_events.csv`；
5. 输出 summary 至 `build/output/smoke_summary.txt`；
6. summary 至少包含输入样本数、allow_measure false 样本数、event 数、IBI min/max、reject reason 统计；
7. 所有输出不得包含 HRV / RMSSD；
8. 禁止动态内存扫描无命中。

## 是否需要 Owner 决策

无。若示例 CSV 尚未放入仓库，Codex 必须报告缺失，不得伪造 `sample_ppg_20000.csv`。

## Owner 下一步

将本包解压覆盖到仓库根目录，并确认仓库存在：

```text
tests/fixtures/sample_ppg_20000.csv
```

然后把 `CODEX_ONE_LINE_COMMAND.md` 中的一句话交给 Codex。
