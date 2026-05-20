# Test Strategy v0.1

## 当前测试定位

由于当前无 ECG 参考、无人工标注、无 gold standard，本阶段只能做 smoke test 和工程合理性验证。

不得宣称算法准确性。

## 示例数据

Owner 已确认示例数据：

```text
示例数据_20000.csv
```

建议入仓库路径：

```text
tests/fixtures/sample_ppg_20000.csv
```

字段：

```text
timestamp_ms, PPG_G1, PPG_G2, PPG_G3, PPG_G4, allow_measure
```

## 基础 smoke test 项

1. CSV 可读取；
2. 必要字段存在；
3. timestamp 间隔约为 20 ms；
4. 4 路 PPG 数据可解析为 `int32_t`；
5. `allow_measure` 可解析；
6. process 可逐点运行；
7. 输出 event 字段完整；
8. IBI 在 300–2000 ms；
9. `allow_measure=false` 时不输出 IBI；
10. 低质量 / 异常时不输出或降低 confidence。

## 不做的测试

当前不做：

1. MAE；
2. RMSE；
3. matched beats；
4. coverage；
5. 与 ECG 参考对齐；
6. 临床准确性证明。

## Python 测试脚本规则

1. Python 只能作为 host 辅助；
2. 只允许标准库；
3. 不得替代 C 算法；
4. 不得依赖 numpy / scipy / pandas 等第三方库。

## 后续建议

M7 可生成 host 评估工具，并输出：

```text
build/output/ibi_events.csv
build/output/smoke_summary.txt
```

具体路径由后续任务文件冻结。
