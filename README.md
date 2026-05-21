# M5 修复轮 1 更新包

把本 ZIP 解压覆盖到仓库根目录，替换 `docs/10_CODEX_NEXT_TASK.md`。

随后给 Codex：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

本轮只修复 M5 detector 的跨通道历史混用和 EVENT_READY 事件字段一致性问题。
