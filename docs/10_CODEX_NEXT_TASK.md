# PR #24 复审追加修复要求 — M8 Fix 7

PR #24 不通过。原因：它只更新了 docs/06_RESOURCE_BUDGET.md，但 make resource-report 失败，错误为 No rule to make target 'resource-report'。

本轮必须生成一个完整自包含的最终 M8 PR，不允许只改文档。

## 必须修复

1. Makefile 必须包含 resource-report target：
   - 编译 tools/ppg_ibi_resource_report.c；
   - 在运行前创建 build/output；
   - 运行后生成 build/output/resource_report.txt。

2. 必须新增或恢复 tools/ppg_ibi_resource_report.c：
   - 调用 ppg_ibi_context_size()；
   - 输出 build/output/resource_report.txt；
   - 输出字段至少包括：
     - context_size_bytes
     - ram_budget_min_bytes
     - ram_budget_max_bytes
     - context_size_within_budget
     - uses_dynamic_memory
     - uses_recursion 如当前工具已有
   - 判断规则必须是：
     context_size_bytes <= ram_budget_max_bytes -> yes

3. 保留 docs/06_RESOURCE_BUDGET.md 的 M8 收敛内容：
   - RAM 预算 15–20 KB；
   - context_size_bytes=76；
   - context_size_within_budget=yes；
   - make resource-report；
   - build/output/resource_report.txt；
   - 无动态内存；
   - 无递归；
   - 无 FPU + float 剩余风险；
   - 后续滑窗 / SQI / detector 缓存扩展需重新评估 RAM。

4. 保留前面 M8 已完成修复：
   - docs/04_IO_CONTRACT.md 的 M8 收敛；
   - docs/07_TEST_STRATEGY.md 的 M8 收敛；
   - docs/08_RISK_REVIEW.md 的 M8 收敛；
   - tools/ppg_ibi_csv_smoke.c 不调用 system("mkdir -p ...")；
   - Makefile 的 csv-smoke target 创建 build/output；
   - CSV 6 列 contract 不回退；
   - public headers 不变；
   - 不引入 HR / HRV / RMSSD 输出；
   - 不引入动态内存。

## 禁止事项

不得修改 public function signatures、ppg_ibi_event_t、status/state/reject enum、采样率、通道数、IBI 范围常量、include/ppg_ibi.h、include/ppg_ibi_config.h。

不得引入 malloc/calloc/realloc、外部依赖或第三方 PPG/IBI/HR/HRV 算法库。

## 必须运行并报告

make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
cat build/output/resource_report.txt
cat build/output/smoke_summary.txt



# PR #23 复审追加修复要求 — M8 Fix 6

PR #23 暂不通过。当前唯一阻塞点是：docs/06_RESOURCE_BUDGET.md 没有完成 M8 资源预算收敛。

## 必须修复

更新 `docs/06_RESOURCE_BUDGET.md` 到 M8 视角，至少包含：

1. RAM 预算：15–20 KB；
2. 当前 `ppg_ibi_context_size()` 实测值，例如 `context_size_bytes=76`；
3. `make resource-report` 命令；
4. `build/output/resource_report.txt` 输出字段：
   - `context_size_bytes`
   - `ram_budget_min_bytes`
   - `ram_budget_max_bytes`
   - `context_size_within_budget`
   - `uses_dynamic_memory`
   - 如当前工具已有，也记录 `uses_recursion`
5. `context_size_within_budget=yes` 的判断依据；
6. 判断规则：`context_size_bytes <= ram_budget_max_bytes` 即为预算内；
7. 当前无动态内存；
8. 当前无递归；
9. 当前 context size 远低于 RAM 上限；
10. 无 FPU 但允许 float 的剩余风险；
11. 后续如引入滑窗、更复杂 SQI、detector 缓存，必须重新评估 RAM。

## 必须保留

不得回退 PR #23 已完成内容：

- `docs/04_IO_CONTRACT.md` 的 M8 收敛；
- `docs/07_TEST_STRATEGY.md` 的 M8 收敛；
- `docs/08_RISK_REVIEW.md` 的 M8 收敛；
- `tools/ppg_ibi_csv_smoke.c` 不调用 `system("mkdir -p ...")`；
- `Makefile` 的 `csv-smoke` / `resource-report` 创建 `build/output`；
- `tools/ppg_ibi_resource_report.c` 输出 resource report；
- public headers 不变；
- 不引入 HR / HRV / RMSSD 输出；
- 不引入动态内存。

## 必须运行并报告

make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
cat build/output/resource_report.txt
cat build/output/smoke_summary.txt


# PR #22 复审追加修复要求 — M8 Fix 5

PR #22 暂不通过。它已经修复了 resource-report、docs/06_RESOURCE_BUDGET.md 和 host 工具目录创建问题，但没有补齐 M8 文档收敛。

## 必须修复

1. 更新 `docs/04_IO_CONTRACT.md` 到 M8 视角：
   - 保留 public headers、核心常量、输入结构、输出结构、config、state enum、reject reason enum、status enum、public C API；
   - 修正旧语义，不得再写“M2 不输出真实 IBI，仅返回 NO_EVENT 占位行为”作为当前语义；
   - 增加当前 `EVENT_READY` 工程语义；
   - 明确 `EVENT_READY` 只是工程闭环，不代表临床准确性；
   - 明确不输出 `hr_bpm` / HRV / RMSSD。

2. 更新 `docs/07_TEST_STRATEGY.md` 到 M8 视角：
   - 保留 `make test` 组成：API compile、input validation、signal quality、pulse detector、state machine；
   - 增加 `make csv-smoke`：输入 `tests/fixtures/sample_ppg_20000.csv`，输出 `ibi_events.csv` 和 `smoke_summary.txt`；
   - 增加 `make resource-report`：输出 `build/output/resource_report.txt`；
   - 列出动态内存扫描、HR/RMSSD 字段扫描、public header diff 检查；
   - 明确无 gold standard 时不做 MAE/RMSE/matched beats/coverage；
   - 明确 CSV smoke 只是工程闭环，不是准确性证明。

3. 更新 `docs/08_RISK_REVIEW.md` 到 M8 视角：
   - 保留风险分级 S0/S1/S2/S3；
   - 修正旧语义，不得再写“当前不返回 EVENT_READY”；
   - 保留或重建 S0 触发条件；
   - 记录无 gold standard、50Hz 限制、无 FPU 但允许 float、外部 allow_measure、最小三点 detector、真实场景漏检/误检风险；
   - 明确当前不能宣称临床准确性。

4. 保留 PR #22 已完成内容：
   - `tools/ppg_ibi_csv_smoke.c` 不调用 `system("mkdir -p ...")`；
   - `Makefile` 的 `csv-smoke` / `resource-report` 创建 `build/output`；
   - `tools/ppg_ibi_resource_report.c` 输出 resource report；
   - `docs/06_RESOURCE_BUDGET.md` 的 M8 资源预算内容；
   - `context_size_within_budget=yes` 当 `context_size_bytes <= ram_budget_max_bytes`。

## 禁止事项

不得修改 public function signatures、`ppg_ibi_event_t`、status/state/reject enum、采样率、通道数、IBI 范围常量、`include/ppg_ibi.h`、`include/ppg_ibi_config.h`。

不得引入 malloc/calloc/realloc、外部依赖或第三方 PPG/IBI/HR/HRV 算法库。

## 必须运行并报告

make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
cat build/output/resource_report.txt
cat build/output/smoke_summary.txt


# PR #21 复审追加修复要求 — M8 Fix 4

PR #21 暂不通过。它修复了 host 工具目录创建问题，但没有保留 PR #20 中对 docs/06_RESOURCE_BUDGET.md 的 M8 资源预算收敛。

## 必须修复

1. 补齐 `docs/06_RESOURCE_BUDGET.md` 到 M8 视角，至少包含：
   - RAM 预算：15–20 KB；
   - 当前 `ppg_ibi_context_size()` 实测值，例如 `context_size_bytes=76`；
   - `make resource-report` 命令；
   - `build/output/resource_report.txt` 输出字段；
   - `context_size_within_budget=yes` 的判断依据；
   - 判断规则：`context_size_bytes <= ram_budget_max_bytes` 即为预算内；
   - 无动态内存；
   - 无递归；
   - 当前 context size 远低于 RAM 上限；
   - 无 FPU 但允许 float 的剩余风险；
   - 后续如引入滑窗 / 更复杂 SQI / detector 缓存，需重新评估 RAM。

2. 保留 PR #21 已完成内容：
   - `tools/ppg_ibi_csv_smoke.c` 不调用 `system("mkdir -p ...")`；
   - `Makefile` 的 `csv-smoke` target 在运行工具前创建 `build/output`；
   - `Makefile` 的 `resource-report` target 在运行工具前创建 `build/output`；
   - `tools/ppg_ibi_resource_report.c` 输出 `context_size_within_budget=yes` 当 `context_size_bytes <= ram_budget_max_bytes`。

3. 不得回退：
   - CSV 6 列 contract；
   - `make csv-smoke` 输出 `ibi_events.csv` 与 `smoke_summary.txt`；
   - `make resource-report` 输出 `resource_report.txt`；
   - public headers 不变；
   - 不引入 HR / HRV / RMSSD 输出；
   - 不引入动态内存。

## 必须运行并报告

make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
cat build/output/resource_report.txt
cat build/output/smoke_summary.txt


# PR #20 复审追加修复要求 — M8 Fix 3

PR #20 暂不通过。它修复了 resource-report 的预算判断和 docs/06_RESOURCE_BUDGET.md，但没有保留 PR #19 已经要求的 host 工具目录创建修复。

## 必须修复

1. `tools/ppg_ibi_csv_smoke.c` 不得调用 `system("mkdir -p ...")`。
2. 如果 `stdlib.h` 仅用于 `system()`，请移除 `#include <stdlib.h>`。
3. `Makefile` 的 `csv-smoke` target 必须在运行工具前执行：

@mkdir -p build/output

4. 保留 PR #20 已完成的 resource-report 修复：
   - `context_size_within_budget=yes` 当 `context_size_bytes <= ram_budget_max_bytes`；
   - `docs/06_RESOURCE_BUDGET.md` 保留 M8 资源复盘内容；
   - `make resource-report` 输出 `build/output/resource_report.txt`。

5. 不得回退：
   - CSV 6 列 contract；
   - `make csv-smoke` 输出 `ibi_events.csv` 与 `smoke_summary.txt`；
   - public headers 不变；
   - 不引入 HR / HRV / RMSSD 输出；
   - 不引入动态内存。

## 必须运行并报告

make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
cat build/output/resource_report.txt
cat build/output/smoke_summary.txt


# PR #19 复审追加修复要求 — M8 Fix 2

PR #19 暂不通过。当前阻塞点：

1. `tools/ppg_ibi_resource_report.c` 的 `context_size_within_budget` 判断逻辑错误；
2. `docs/06_RESOURCE_BUDGET.md` 未完成 M8 资源预算收敛。

## 必须修复

### 1. 修复 resource report 预算判断

当前 RAM 预算含义是：算法 context 必须不超过可用 RAM 预算上限。

因此当：

```text
context_size_bytes=76
ram_budget_max_bytes=20480

应输出：

context_size_within_budget=yes



# PR #18 复审追加修复要求 — M8 Fix 1

PR #18 暂不通过。当前阻塞点是：M8 文档收敛过度压缩，导致关键 IO Contract / 测试策略 / 风险约束不再具备权威性；同时 host 工具中仍使用 `system("mkdir -p ...")`，不利于 M8 的可移植性和 MISRA 风格收敛。

请在当前 M8 PR 分支继续修复，并不得回退 PR #18 已完成的：

- `make test`；
- `make csv-smoke`；
- `make resource-report`；
- `build/output/ibi_events.csv`；
- `build/output/smoke_summary.txt`；
- `build/output/resource_report.txt`；
- public headers 不变；
- 不引入 HR / HRV / RMSSD 输出；
- 不引入动态内存。

## 本轮必须修复

### 1. 恢复 / 补齐 `docs/04_IO_CONTRACT.md` 的权威内容

`docs/04_IO_CONTRACT.md` 不应只写“维持既有字段”。必须重新列出至少以下内容：

- public headers；
- 核心常量：通道数、采样率、expected interval、IBI 范围；
- `ppg_ibi_sample_t` 字段；
- `ppg_ibi_event_t` 字段；
- `ppg_ibi_config_t` 字段；
- state enum；
- reject reason enum；
- status enum；
- public C API；
- `EVENT_READY` 当前工程语义；
- 明确不输出 `hr_bpm` / HRV / RMSSD；
- 明确 `EVENT_READY` 不代表临床准确性验证完成。

可以在旧版文档基础上追加 M8 说明，不要把权威字段表压缩掉。

### 2. 恢复 / 补齐 `docs/07_TEST_STRATEGY.md` 的分阶段测试依据

`docs/07_TEST_STRATEGY.md` 不应只保留三条主命令。必须至少覆盖：

- `make test` 的组成：API compile、input validation、signal quality、pulse detector、state machine；
- `make csv-smoke` 的输入、输出和 summary 字段；
- `make resource-report` 的输出字段；
- 禁止动态内存扫描；
- 禁止 HR/RMSSD 输出字段扫描；
- public header diff 检查；
- 无 gold standard 情况下不做 MAE/RMSE/matched beats/coverage；
- CSV smoke 仅是工程闭环，不是准确性证明。

### 3. 恢复 / 补齐 `docs/08_RISK_REVIEW.md` 的风险与 S0 触发条件

`docs/08_RISK_REVIEW.md` 必须保留或重建：

- 风险分级 S0/S1/S2/S3；
- 当前风险清单；
- S0 触发条件，例如改变 public API、改变输出字段、引入第三方算法库、引入动态内存、改变采样率/门控语义、医疗诊断用途；
- M8 风险结论；
- 无 gold standard、50Hz 采样、无 FPU、外部 allow_measure、最小三点 detector 的限制。

### 4. 修复 host 工具的目录创建方式

`tools/ppg_ibi_csv_smoke.c` 和 `tools/ppg_ibi_resource_report.c` 中不要调用 `system("mkdir -p ...")`。

建议：

- 由 `Makefile` 的 `csv-smoke` 和 `resource-report` target 负责 `@mkdir -p build/output`；
- 工具只负责打开并写入输出文件；
- 如果输出目录不存在，工具可以报错，但不应调用 shell 命令。

修复后，如 `stdlib.h` 仅因 `system()` 而存在，应移除不需要的 `#include <stdlib.h>`。

### 5. 保持源码行为不回退

不得修改 public function signatures、`ppg_ibi_event_t`、status/state/reject enum、采样率、通道数、IBI 范围常量、`include/ppg_ibi.h`、`include/ppg_ibi_config.h`。

不得修改 M6/M7 已验证行为：

- strict reject -> REACQUIRE；
- allow_measure=false -> HOLD；
- IBI_OUT_OF_RANGE -> REACQUIRE；
- EVENT_READY 前 history 连续性；
- CSV 6 列 contract；
- `make csv-smoke` 输出 events 与 summary；
- `make resource-report` 输出 resource report。

## 必须运行并报告

```bash
make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
head -1 tests/fixtures/sample_ppg_20000.csv
cat build/output/smoke_summary.txt
cat build/output/resource_report.txt
```

## 交付要求

Codex 输出必须报告：

- Changed files；
- Test commands；
- Test results；
- 是否修改 public API；
- 是否引入动态内存；
- 是否引入外部依赖；
- 是否执行 Git 操作。



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
