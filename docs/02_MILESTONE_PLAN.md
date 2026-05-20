# Milestone Plan v0.1

## M0：项目启动问诊

状态：已完成。

产出：Project Brief v0.1 草案，并经 Owner 确认。

## M1：项目启动包与文档骨架

状态：当前阶段。

目标：生成仓库文档骨架，不实现源码。

产出：

1. `docs/00_PROJECT_BRIEF.md`；
2. `docs/01_DECISION_LOG.md`；
3. `docs/02_MILESTONE_PLAN.md`；
4. `docs/03_ALGORITHM_SCOPE.md`；
5. `docs/04_IO_CONTRACT.md`；
6. `docs/05_MCU_ALGORITHM_STRATEGY.md`；
7. `docs/06_RESOURCE_BUDGET.md`；
8. `docs/07_TEST_STRATEGY.md`；
9. `docs/08_RISK_REVIEW.md`；
10. `docs/09_CODEX_RUNBOOK.md`；
11. `docs/10_CODEX_NEXT_TASK.md`；
12. `docs/11_GIT_WORKFLOW.md`；
13. `AGENTS.md`。

不做：源码骨架、算法实现、测试代码、CI。

## M2：IO Contract、MCU API 与资源约束冻结

建议目标：

1. 冻结 C API 设计；
2. 冻结输入 / 输出结构体；
3. 冻结状态枚举和 reject reason；
4. 明确 context 由 caller 分配；
5. 明确 RAM 预算和禁止动态内存规则；
6. 可选择生成最小 C API 骨架，但不实现算法。

是否进入 M2 需 Owner 确认。

## M3：输入采样校验与基础数据路径

建议目标：

1. 逐点输入；
2. timestamp 间隔校验；
3. sample_counter；
4. 饱和 / 丢样 / 异常标记；
5. 不做复杂滤波和 IBI 检测。

## M4：基础预处理与 SQI / 主通道选择

建议目标：

1. 轻量预处理；
2. 基础 SQI；
3. 主通道选择；
4. 低质量拒绝策略。

## M5：脉搏候选检测与 IBI 输出

建议目标：

1. 自研轻量峰值 / 周期候选检测；
2. IBI 合法性检查；
3. IBI event 输出；
4. smoke test。

## M6：状态机、运动门控与异常处理

建议目标：

1. 完整状态机；
2. `allow_measure` 门控；
3. REACQUIRE 策略；
4. reject reason 完整性。

## M7：host 端评估工具与示例数据 smoke test

建议目标：

1. 使用 `tests/fixtures/sample_ppg_20000.csv`；
2. 输出事件 CSV；
3. 检查 IBI 范围、输出字段、门控逻辑；
4. 不做准确性指标。

## M8：资源预算审查与 MCU 移植准备

建议目标：

1. 静态内存估算；
2. 栈使用审查；
3. 禁止动态内存检查；
4. MCU 移植风险复盘；
5. float 使用风险复盘。
