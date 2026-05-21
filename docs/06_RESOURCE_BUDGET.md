# Resource Budget v0.3 (M8)

## 已确认预算

- RAM 预算：15–20 KB（15360–20480 bytes）
- FPU：Owner 当前声明无 FPU
- float：允许，但需持续复盘无 FPU 代价
- 动态内存：禁止

## M8 实测结论

- `ppg_ibi_context_size()` 当前实测：`76 bytes`（由 `make resource-report` 输出）。
- 对比预算：`76 bytes` 明显低于 15–20 KB RAM 预算上限。
- 库内动态内存：无 `malloc/calloc/realloc`。
- 大栈数组：当前核心与 smoke/resource 工具均未引入不可控大栈数组。
- 算法复杂度：逐点 O(1)，无递归。

## 资源输出文件

- 命令：`make resource-report`
- 输出：`build/output/resource_report.txt`
- 字段：
  - `context_size_bytes`
  - `ram_budget_min_bytes`
  - `ram_budget_max_bytes`
  - `context_size_within_budget`
  - `uses_dynamic_memory`

## M8 后仍需复盘风险

1. 无 FPU 平台上的 float 指令开销（延时/功耗）仍需在真实 MCU profile 中验证。
2. 若后续引入更复杂 SQI/检测器历史缓存，需持续跟踪 context 增长。
3. 目前仅完成工程闭环，不代表真实数据场景下的最终资源峰值。
