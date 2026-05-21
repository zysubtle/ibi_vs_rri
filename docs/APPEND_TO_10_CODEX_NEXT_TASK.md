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
