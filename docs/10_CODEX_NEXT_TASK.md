# M8 Final Consolidation Task

目标：生成一个完整、自包含、可合并的 M8 最终收敛 PR。

当前 PR #25 暂不通过。原因：它只保留了 resource-report / docs/06_RESOURCE_BUDGET.md / csv-smoke 目录创建修复，但没有保留 M8 对 docs/04_IO_CONTRACT.md、docs/07_TEST_STRATEGY.md、docs/08_RISK_REVIEW.md 的最终文档收敛。

## 必须保留 PR #25 已完成内容

1. Makefile 必须包含：
   - csv-smoke target；
   - resource-report target；
   - csv-smoke 运行前创建 build/output；
   - resource-report 运行前创建 build/output。

2. tools/ppg_ibi_csv_smoke.c：
   - 不得调用 system("mkdir -p ...")；
   - 不得包含仅用于 system() 的 stdlib.h；
   - 保持 CSV 6 列 contract；
   - 保持输出 build/output/ibi_events.csv 与 build/output/smoke_summary.txt。

3. tools/ppg_ibi_resource_report.c：
   - 必须存在；
   - 调用 ppg_ibi_context_size()；
   - 输出 build/output/resource_report.txt；
   - 输出字段至少包括：
     - context_size_bytes
     - ram_budget_min_bytes
     - ram_budget_max_bytes
     - context_size_within_budget
     - uses_dynamic_memory
     - uses_recursion
   - 判断规则：
     context_size_bytes <= ram_budget_max_bytes -> context_size_within_budget=yes

4. docs/06_RESOURCE_BUDGET.md：
   - 保持 M8 资源预算收敛；
   - 包含 RAM 15–20 KB；
   - 包含 context_size_bytes=76；
   - 包含 context_size_within_budget=yes；
   - 包含 make resource-report；
   - 包含 build/output/resource_report.txt；
   - 包含无动态内存、无递归、无 FPU + float 剩余风险。

## 必须补齐 M8 文档收敛

### 1. docs/04_IO_CONTRACT.md

必须更新到 M8 视角，至少包含：

- public headers；
- 核心常量；
- ppg_ibi_sample_t；
- ppg_ibi_event_t；
- ppg_ibi_config_t；
- state enum；
- reject reason enum；
- status enum；
- public C API；
- 当前 EVENT_READY 工程语义；
- 明确 EVENT_READY 只代表工程闭环，不代表临床准确性；
- 明确不输出 hr_bpm / HRV / RMSSD；
- 删除或修正“当前仅返回 NO_EVENT 占位行为”这类旧语义。

### 2. docs/07_TEST_STRATEGY.md

必须更新到 M8 视角，至少包含：

- make test 组成：
  - API compile；
  - input validation；
  - signal quality；
  - pulse detector；
  - state machine；
- make csv-smoke：
  - 输入 tests/fixtures/sample_ppg_20000.csv；
  - 输出 build/output/ibi_events.csv；
  - 输出 build/output/smoke_summary.txt；
  - summary 至少包含 parsed_samples、invalid_lines、allow_measure_false_samples、event_ready_count、final_status；
- make resource-report：
  - 输出 build/output/resource_report.txt；
  - 字段包含 context_size_bytes、ram_budget_min_bytes、ram_budget_max_bytes、context_size_within_budget、uses_dynamic_memory、uses_recursion；
- 动态内存扫描；
- HR/RMSSD 字段扫描；
- public header diff 检查；
- 无 gold standard 时不做 MAE / RMSE / matched beats / coverage；
- CSV smoke 只是工程闭环，不是准确性证明。

### 3. docs/08_RISK_REVIEW.md

必须更新到 M8 视角，至少包含：

- 风险分级 S0/S1/S2/S3；
- 当前风险清单；
- 无 gold standard 风险；
- 50Hz 采样限制；
- 无 FPU 但允许 float 风险；
- 外部 allow_measure 风险；
- 最小三点 detector 在真实场景下可能漏检/误检；
- 工程 EVENT_READY 不代表临床准确性；
- S0 触发条件：
  - 改变 public API；
  - 改变输出字段语义；
  - 引入第三方 PPG/IBI/HR/HRV 算法库；
  - 引入动态内存；
  - 改变采样率、通道数、运动门控语义或状态机核心语义；
  - 用于医疗诊断。

## 禁止事项

不得修改：

- public function signatures；
- ppg_ibi_event_t；
- status/state/reject enum；
- 采样率；
- 通道数；
- IBI 范围常量；
- include/ppg_ibi.h；
- include/ppg_ibi_config.h。

不得引入：

- malloc/calloc/realloc；
- 外部依赖；
- 第三方 PPG/IBI/HR/HRV 算法库；
- hr_bpm / HRV / RMSSD 输出字段。

## 必须运行并报告

make test
make csv-smoke
make resource-report
rg -n "\\b(malloc|calloc|realloc)\\s*\\(" include src tests tools
rg -n "hr_bpm|rmssd|RMSSD" include src tests tools
git diff -- include/ppg_ibi.h include/ppg_ibi_config.h
cat build/output/resource_report.txt
cat build/output/smoke_summary.txt

## Codex 输出必须报告

- Changed files；
- Test commands；
- Test results；
- 是否修改 public API；
- 是否引入动态内存；
- 是否引入外部依赖；
- 是否执行 Git 操作。