# M5 启动更新包 — MCU PPG-IBI

本包用于在 Owner 已确认“M4 已合并，进入 M5”后，更新仓库中的当前任务文件。

请将本 ZIP 解压覆盖到仓库根目录。

本包包含：

```text
docs/15_M5_START_CARD.md
docs/10_CODEX_NEXT_TASK.md
CODEX_ONE_LINE_COMMAND.md
README.md
```

## 重要说明

M5 需要检测历史状态，因此本包中的 `docs/10_CODEX_NEXT_TASK.md` 明确授权 Codex 修改 `include/ppg_ibi.h`，但仅限扩展 `ppg_ibi_context_t` 的内部检测状态字段。

不得修改 public function signature、输入结构体、输出结构体、status enum、state enum、reject enum 或已冻结字段语义。

如果 Owner 不接受 context 扩展，请不要把本任务交给 Codex。

## 给 Codex 的一句话指令

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## M5 目标

实现最小自研脉搏候选检测与 IBI event 输出闭环，在 synthetic host test 中验证 `PPG_IBI_STATUS_EVENT_READY` 和合法 `ibi_ms`。

## M5 不做

不做 HRV/RMSSD，不输出 hr_bpm，不读取示例 CSV，不做准确性评估，不引入第三方算法库，不使用动态内存。
