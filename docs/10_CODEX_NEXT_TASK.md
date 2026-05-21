# docs/10_CODEX_NEXT_TASK.md — M8 任务文件

## 当前 Milestone
M8：资源预算审查、代码可读性 / MISRA 风格整理、文档收敛与风险复盘。

## 背景
M7 已合并。项目当前已具备：

1. 最小 MCU C API；
2. 逐点 PPG 输入；
3. timestamp / sample counter / 24-bit raw 基础校验；
4. 最小 SQI 与主通道选择；
5. 三点局部峰候选与最小 IBI event 输出；
6. 状态机与异常恢复语义；
7. host 端 CSV smoke test 与输出 summary。

M8 不继续扩展算法能力，而是做工程阶段收敛。

---

## 本轮任务目标

### 目标 1：代码可读性 / MISRA 风格整理

对以下文件做必要的可读性整理：

```text
src/ppg_ibi.c
tools/ppg_ibi_csv_smoke.c
```

要求：

1. 拆分一行多语句；
2. 拆分过长的 `if` / `switch` / 函数行；
3. 保持小函数清晰；
4. 避免行为改变；
5. 不新增动态内存；
6. 不改变 public API；
7. 不改变状态机语义；
8. 不改变 EVENT_READY、IBI_OUT_OF_RANGE、strict reject 的行为。

如需添加 helper function，只能是内部 `static` helper。

### 目标 2：资源预算复盘

更新或补充资源预算文档：

```text
docs/06_RESOURCE_BUDGET.md
```

至少包含：

1. 当前 `ppg_ibi_context_size()` 的实际字节数；
2. RAM 预算 15–20 KB 的结论；
3. 当前是否使用动态内存；
4. 当前是否存在大栈数组；
5. 当前 float 使用状态；
6. M8 后仍需复盘的资源风险。

建议新增 host 工具或 Makefile 目标：

```text
make resource-report
```

如果新增该目标，输出建议为：

```text
build/output/resource_report.txt
```

内容至少包括：

```text
context_size_bytes=...
ram_budget_min_bytes=15360
ram_budget_max_bytes=20480
context_size_within_budget=yes/no
uses_dynamic_memory=no
```

### 目标 3：文档收敛

更新以下文档，使其与 M1–M7 实际状态一致：

```text
docs/02_MILESTONE_PLAN.md
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

文档重点：

1. M0–M7 已完成，M8 为收敛阶段；
2. 当前已有最小 `EVENT_READY` 工程语义；
3. 当前不输出 HR / HRV / RMSSD；
4. 当前 CSV smoke test 只是工程闭环，不是准确性评估；
5. 当前无 ECG / 人工标注 / gold standard；
6. 当前 detector 仍为最小 synthetic-first 三点局部峰策略；
7. 当前 risk review 不得宣称临床准确性。

### 目标 4：阶段验收说明

新增或更新一个阶段验收文档，建议路径：

```text
docs/12_M8_ACCEPTANCE_REPORT.md
```

内容至少包括：

1. 当前版本完成了哪些工程能力；
2. 当前测试命令；
3. 当前 smoke test 输出路径；
4. 当前不做什么；
5. 当前主要风险；
6. 后续建议：进入真实数据评估 / 算法增强前，需要 gold standard 或标注数据。

---

## 本轮非目标

本轮不得实现以下内容：

1. 不新增滤波、DC removal、滑动窗口 SQI、模板匹配、FFT、ACF；
2. 不新增复杂峰值检测策略；
3. 不调参以追求真实数据准确率；
4. 不读取新的外部数据集；
5. 不输出 HR、HRV、RMSSD；
6. 不新增 GUI；
7. 不新增 Python 第三方依赖；
8. 不引入 CMSIS-DSP；
9. 不修改 public API；
10. 不修改 `tests/fixtures/sample_ppg_20000.csv`，除非发现其格式与 M7 contract 不一致，并必须在 summary 中说明。

---

## 允许修改的文件 / 目录

```text
src/ppg_ibi.c
tools/ppg_ibi_csv_smoke.c
Makefile
docs/02_MILESTONE_PLAN.md
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
docs/10_CODEX_NEXT_TASK.md
docs/12_M8_ACCEPTANCE_REPORT.md
```

如新增资源报告工具，可新增：

```text
tools/ppg_ibi_resource_report.c
```

如新增 resource output，可由测试运行生成：

```text
build/output/resource_report.txt
```

`build/` 产物不要提交。

---

## 禁止修改的文件 / 范围

未经 Owner 明确 S0 决策，不得修改：

```text
include/ppg_ibi.h
include/ppg_ibi_config.h
```

不得修改 public function signatures、`ppg_ibi_event_t`、status/state/reject enum、采样率、通道数、IBI 范围常量。

不得修改 M6 已收敛语义：

1. `allow_measure=false -> HOLD + reset detector/last pulse`；
2. `SATURATED/TIMESTAMP_GAP/SAMPLE_DROP/LOW_SIGNAL_QUALITY -> REACQUIRE + reset detector/last pulse`；
3. `IBI_OUT_OF_RANGE -> REACQUIRE + reset detector/last pulse`；
4. `EVENT_READY` 字段对齐 pulse candidate；
5. `EVENT_READY` 返回前推进 history，避免下一拍重复消费同一 candidate。

---

## 测试命令

必须运行并报告：

```bash
make test
make csv-smoke
```

如果新增 resource-report 目标，必须运行并报告：

```bash
make resource-report
```

必须运行并报告：

```bash
rg -n "\b(malloc|calloc|realloc)\s*\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
head -1 tests/fixtures/sample_ppg_20000.csv
cat build/output/smoke_summary.txt
```

如果 `rg` 不存在，可使用等价 `grep -R -E`，但必须说明。

---

## 通过标准

1. `make test` 通过；
2. `make csv-smoke` 通过；
3. 若新增 `make resource-report`，该目标通过；
4. `build/output/ibi_events.csv` 存在；
5. `build/output/smoke_summary.txt` 存在；
6. summary 中 `parsed_samples > 0`；
7. summary 中包含 `event_ready_count`、`invalid_lines`、`allow_measure_false_samples`、`final_status`；
8. 动态内存扫描无命中；
9. HR/RMSSD 禁用字段扫描无命中；
10. public headers 无差异；
11. 文档与实际源码 / 测试状态一致；
12. 没有新增第三方依赖；
13. 没有 Git 远程操作。

---

## 失败时必须报告

如果失败，请报告：

1. 哪个命令失败；
2. 失败日志关键行；
3. 是否涉及 public API；
4. 是否需要 Owner S0 决策；
5. 修改文件列表；
6. 当前是否存在未提交变更。

---

## Codex 输出摘要要求

完成后请输出：

```text
Summary
Changed files
Test commands
Test results
Known limitations
Constraint checklist
是否修改 API
是否修改输出字段
是否修改 function signature / enum
是否引入动态内存
是否引入外部依赖
是否执行 Git 操作
commit hash（如有）
```

---

## Git / PR 限制

Codex 不得执行：

```text
push
创建 PR
merge
rebase
squash merge
删除远程分支
修改 main/dev 历史
```

如果本地 commit，必须报告 commit hash。
