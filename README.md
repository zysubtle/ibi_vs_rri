# M2 启动更新包 — MCU PPG-IBI

本包用于在 Owner 已确认“M1 已放入仓库，进入 M2”后，更新仓库中的当前任务文件。

请将本 ZIP 解压覆盖到仓库根目录。

本包只包含 M2 启动说明和 `docs/10_CODEX_NEXT_TASK.md` 的完整替换版本，不包含源码实现。

完成覆盖后，给 Codex 的一句话指令是：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

M2 目标：冻结 IO Contract、MCU C API 与资源约束，并生成最小 C API 骨架与编译测试。
