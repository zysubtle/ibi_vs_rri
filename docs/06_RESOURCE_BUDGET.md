# Resource Budget v0.3 (M8)

## 已确认预算

| 项目 | 预算 / 状态 |
|---|---|
| MCU | Apollo3.5 |
| FPU | Owner 当前声明无 FPU |
| RAM | 15–20 KB（15360–20480 bytes） |
| ROM | 暂不考虑 |
| float | 允许（需持续记录无 FPU 风险） |
| malloc/calloc/realloc | 禁止 |
| MISRA 风格 | 需要 |

## M8 实测结论

基于 `make resource-report` 当前输出：

- `context_size_bytes=76`
- `ram_budget_min_bytes=15360`
- `ram_budget_max_bytes=20480`
- `context_size_within_budget=yes`
- `uses_dynamic_memory=no`
- `uses_recursion=no`

结论：当前 `ppg_ibi_context_size()` 为 76 bytes，远低于 RAM 上限 20480 bytes，处于预算内。

## 预算内判断规则

统一判断规则：

```text
context_size_within_budget = (context_size_bytes <= ram_budget_max_bytes) ? yes : no
```

因此在当前数据下：

```text
76 <= 20480 -> yes
```

## 当前资源状态复盘

1. `ppg_ibi_context_t` 由调用方分配，库内无动态内存；
2. 当前无递归调用；
3. 当前无不可控大栈数组（核心路径为逐点 O(1)）；
4. 当前 float 使用仍受“无 FPU”约束，未证明在最差实时负载下的功耗/时延边界；
5. 现阶段 context size 对 RAM 预算压力极低。

## 检查命令与输出

- 命令：`make resource-report`
- 输出文件：`build/output/resource_report.txt`

输出字段至少包含：

- `context_size_bytes`
- `ram_budget_min_bytes`
- `ram_budget_max_bytes`
- `context_size_within_budget`
- `uses_dynamic_memory`
- `uses_recursion`

## M8 后续复盘触发条件

若后续引入以下能力，必须重新评估 RAM 与时延：

1. 更长滑动窗口（SQI/滤波缓存）；
2. 更复杂 SQI 特征与多阶段判别状态；
3. detector 历史/候选缓存扩展；
4. 更多 debug 统计常驻 context。

同时需继续跟踪无 FPU 平台上的 float 运行成本风险。
