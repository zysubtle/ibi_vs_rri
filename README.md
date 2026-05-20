# M4 启动更新包 — MCU PPG-IBI

本包用于在 Owner 已确认“M3 已合并，进入 M4”后，更新仓库中的当前任务文件。

请将本 ZIP 解压覆盖到仓库根目录。

本包只包含 M4 启动说明和 `docs/10_CODEX_NEXT_TASK.md` 的完整替换版本，不包含源码实现。

完成覆盖后，给 Codex 的一句话指令是：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

M4 目标：在不改变 public API 的前提下，实现最小 raw-range SQI、主通道选择和对应 host 测试。

注意：M4 仍不输出真实 IBI，不返回 `PPG_IBI_STATUS_EVENT_READY`，不做真实滤波、峰值检测或 HR/HRV/RMSSD 输出。
