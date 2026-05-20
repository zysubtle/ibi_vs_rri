# M3 启动更新包 — MCU PPG-IBI

本包用于在 Owner 已确认“M2 已合并，进入 M3”后，更新仓库中的当前任务文件。

请将本 ZIP 解压覆盖到仓库根目录。

本包包含：

```text
docs/13_M3_START_CARD.md
docs/10_CODEX_NEXT_TASK.md
CODEX_ONE_LINE_COMMAND.md
README.md
```

本包不包含源码实现。

覆盖后，给 Codex 的一句话指令是：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

M3 目标：输入采样校验与基础数据路径，包括 timestamp、sample_counter、allow_measure、PPG raw 范围、饱和和丢样标记；不做滤波、SQI、峰值检测或真实 IBI。
