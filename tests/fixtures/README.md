# tests/fixtures

本目录用于放置 Codex 和 host smoke test 可稳定访问的测试数据。

Owner 已确认示例 CSV 可进入仓库。

请将实际文件放置为：

```text
tests/fixtures/sample_ppg_20000.csv
```

原始文件名：

```text
示例数据_20000.csv
```

期望字段：

```text
timestamp_ms, PPG_G1, PPG_G2, PPG_G3, PPG_G4, allow_measure
```

注意：

1. 本 M1 包不包含实际 CSV；
2. Codex 测试必须使用仓库相对路径；
3. 不得引用 ChatGPT 附件或 Sources 中的数据文件。
