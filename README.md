# M6 启动更新包 — MCU PPG-IBI

本包用于在 Owner 已确认“M5 已合并，进入 M6”后，更新仓库中的当前任务文件。

## 使用方式

请将本 ZIP 解压覆盖到仓库根目录。

本包只包含 M6 启动说明和 `docs/10_CODEX_NEXT_TASK.md` 的完整替换版本，不包含源码实现。

覆盖后，给 Codex 的一句话指令是：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## 本包文件

```text
docs/16_M6_START_CARD.md
docs/10_CODEX_NEXT_TASK.md
CODEX_ONE_LINE_COMMAND.md
README.md
```

## M6 摘要

M6 目标是状态机、运动门控与异常恢复语义收敛。重点包括：

1. `INIT / ACQUIRE / TRACK / HOLD / REACQUIRE` 转移规则；
2. strict reject 后 detector history 和 last pulse 清理；
3. `EVENT_READY` 后 detector history 行为连续性；
4. `IBI_OUT_OF_RANGE` 的状态行为；
5. `src/ppg_ibi.c` 的基本可读性 / MISRA 风格整理。
