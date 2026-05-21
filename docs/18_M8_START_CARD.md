# M8 里程碑启动卡：资源预算审查、可读性收敛与风险复盘

## 当前里程碑
M8：资源预算审查、代码可读性 / MISRA 风格整理、文档收敛与风险复盘。

## 本阶段目的
M8 的目的不是继续扩展 PPG-IBI 算法能力，而是把 M1–M7 已完成的工程闭环做阶段性收敛：确认资源预算、整理可读性、补齐文档、确认风险边界，并形成可合并的阶段验收状态。

## 为什么现在做
M7 已完成 host 端 CSV smoke test、fixture 读取、IBI event 导出和 summary 输出。当前项目已经具备最小 MCU C API、输入校验、SQI / 主通道选择、最小 IBI event 输出、状态机恢复和 CSV smoke test。进入下一轮真实算法增强或数据验证前，需要先收敛工程质量和文档一致性。

## 本阶段产出
1. `src/ppg_ibi.c` 和必要 host 工具的可读性 / MISRA 风格整理，前提是不改变行为。
2. 资源预算复盘，至少包含 `ppg_ibi_context_size()` 的实际值和 RAM 预算判断。
3. M7 smoke test 与 M8 验收策略在文档中收敛。
4. 风险复盘：无 gold standard、50Hz 精度限制、无 FPU 但允许 float、最小 detector 误检 / 漏检风险。
5. 阶段性验收报告或文档备注，明确当前项目可作为工程 smoke prototype，但不做准确性声明。

## 本阶段不做
1. 不新增滤波、DC removal、滑动窗口 SQI、模板匹配、FFT、ACF 或复杂检测算法。
2. 不修改 public API、`ppg_ibi_event_t`、函数签名、enum、采样率、通道数、IBI 范围常量。
3. 不修改 M6 已收敛的状态机语义。
4. 不输出 `hr_bpm`、HRV、RMSSD 或任何诊断结论。
5. 不引入第三方算法库、动态内存或外部依赖。

## 验收标准
1. `make test` 通过。
2. `make csv-smoke` 通过，并生成 `build/output/ibi_events.csv` 与 `build/output/smoke_summary.txt`。
3. 如果新增 resource report 目标，则 `make resource-report` 通过，并输出 context size / RAM 判断。
4. 动态内存扫描无命中：`malloc/calloc/realloc` 不出现在 include/src/tests/tools。
5. 禁用输出字段扫描无命中：`hr_bpm/rmssd/RMSSD` 不出现在 include/src/tests/tools。
6. `include/ppg_ibi.h` 和 `include/ppg_ibi_config.h` 无 public API 变更，除非 Codex 先报告 S0。

## 是否需要 Owner 决策
无。若 Codex 认为必须改变 public API、输出字段、状态机语义、算法策略、采样率、动态内存规则或远程 Git 策略，必须暂停并报告为 S0。

## Owner 下一步
把本 ZIP 解压覆盖到仓库根目录，并让 Codex 读取 `docs/10_CODEX_NEXT_TASK.md` 执行 M8。
