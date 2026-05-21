# M5 里程碑启动卡

## 当前里程碑

M5：脉搏候选检测与 IBI event 输出的最小实现

## 本阶段目的

在 M2–M4 已完成最小 API、输入校验、allow_measure 门控、timestamp / sample-drop 标记、24-bit 饱和标记、最小 SQI 与主通道选择的基础上，实现第一个可测试的“有效 IBI event 输出”路径。

M5 的目标不是做高精度 HRV，也不是证明算法准确性；目标是建立 MCU C 库中从“候选脉搏 → IBI 合法性检查 → EVENT_READY 输出”的最小闭环。

## 为什么现在做

M4 已经让 `selected_channel` 与 `signal_quality` 有稳定含义。下一步需要在不引入第三方算法库、不输出 HRV/RMSSD、不依赖示例 CSV gold standard 的前提下，实现一个保守的自研脉搏候选检测路径，让 `ibi_ms` 在简单合成测试中可以产生有效事件。

## 本阶段产出

1. 最小三点局部峰候选检测逻辑；
2. 最小 IBI 合法性检查与 `PPG_IBI_STATUS_EVENT_READY` 输出；
3. 受控扩展 `ppg_ibi_context_t` 以保存检测状态；
4. 新增 synthetic host 测试，验证有效 IBI、过短 IBI、门控重置和 no-event 行为；
5. 更新 IO Contract、Resource Budget、Test Strategy、Risk Review 到 M5 视角。

## 本阶段不做

1. 不做真实 HRV / RMSSD 计算；
2. 不输出 `hr_bpm`；
3. 不做复杂滤波、DC removal、滑动窗口 SQI、FFT、ACF；
4. 不读取示例 CSV，不做真实数据准确性评估；
5. 不引入第三方 PPG / IBI / HR / HRV 算法库；
6. 不使用 malloc/calloc/realloc；
7. 不使用 CMSIS-DSP；
8. 不做完整状态机重构；
9. 不做 MCU 交叉编译。

## 验收标准

1. `make test` 通过；
2. M2/M3/M4 既有测试仍通过；
3. 新增 M5 synthetic pulse detector test 通过；
4. 合成两次有效峰之间能输出一次 `EVENT_READY`，`ibi_ms` 在 300–2000 ms；
5. `allow_measure=false` 不输出 IBI，并清理检测状态，避免跨运动段输出虚假 IBI；
6. 过短 / 过长 IBI 不输出有效 event；
7. 未输出 HR、HRV、RMSSD；
8. 未使用动态内存、第三方算法库或远程 Git 操作。

## 是否需要 Owner 决策

M5 需要一个受控授权：允许 Codex 修改 `include/ppg_ibi.h`，但仅限于扩展 `ppg_ibi_context_t` 的内部检测状态字段。

不得修改 public function signature、输入结构体、输出结构体、status enum、state enum、reject enum 或既有字段语义。

如果 Owner 不接受 context 扩展，应暂停 M5，不要把本任务交给 Codex。

## Owner 下一步

如果接受上述受控 context 扩展，请将本 ZIP 解压覆盖到仓库根目录，然后把以下一句话交给 Codex：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## docs/10_CODEX_NEXT_TASK.md

本包已提供完整替换版本。
