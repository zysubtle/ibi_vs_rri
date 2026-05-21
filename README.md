# M7 启动更新包 — MCU PPG-IBI

本包用于 Owner 确认“进入 M7”后覆盖仓库当前任务文件。

## 使用方式

1. 解压本 ZIP 到仓库根目录。
2. 确认仓库中存在：

```text
tests/fixtures/sample_ppg_20000.csv
```

3. 将 `CODEX_ONE_LINE_COMMAND.md` 中的一句话交给 Codex。

## 注意

本包不包含示例 CSV 数据，不生成源码，只提供 M7 启动卡和 `docs/10_CODEX_NEXT_TASK.md`。

如果示例 CSV 不存在，Codex 不得伪造该文件，应报告缺失。
