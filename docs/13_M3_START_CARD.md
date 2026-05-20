# M3 里程碑启动卡

## 当前里程碑

M3：输入采样校验与基础数据路径

## 本阶段目的

在 M2 已冻结最小 C API 的基础上，补齐逐点输入的数据路径与基础异常标记，使 `ppg_ibi_process()` 对 timestamp、sample_counter、allow_measure、PPG raw 范围、饱和和丢样等情况具有确定行为。

## 为什么现在做

M2 只完成 API 冻结和占位实现。进入滤波、SQI、峰值检测和 IBI 输出前，必须先保证输入样本路径、异常拒绝逻辑和基础状态输出稳定，否则后续算法模块会建立在不可靠的数据入口上。

## 本阶段产出

1. `ppg_ibi_process()` 的基础输入校验与异常 reject 行为；
2. timestamp 间隔、sample_counter、allow_measure 门控的确定行为；
3. PPG 24-bit signed raw 范围、饱和、通道异常的基础标记；
4. M3 host 单元测试 / smoke test；
5. `docs/07_TEST_STRATEGY.md`、`docs/08_RISK_REVIEW.md` 的 M3 更新。

## 本阶段不做

1. 不做滤波；
2. 不做 SQI 分数计算；
3. 不做主通道选择；
4. 不做峰值检测；
5. 不输出真实 IBI / HR / HRV / RMSSD。

## 验收标准

1. `make test` 通过；
2. 不修改公开 API 和输出字段；
3. allow_measure=false、timestamp gap、sample drop、raw 饱和、raw 越界均有确定 no-event 行为；
4. 未使用 `malloc/calloc/realloc`，未引入外部依赖；
5. 仍不实现真实 IBI 检测。

## 是否需要 Owner 决策

无。Owner 已确认 M2 合并并进入 M3。若 Codex 认为需要改变公开 API、输出字段、资源约束、采样率或远程 Git 策略，必须暂停并作为 S0 报告。

## Owner 下一步

将本 ZIP 解压覆盖到仓库根目录，然后把以下一句话交给 Codex：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## docs/10_CODEX_NEXT_TASK.md

本包已提供完整替换版本。
