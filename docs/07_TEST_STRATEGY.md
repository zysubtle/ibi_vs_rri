# Test Strategy v0.4

## 当前定位

无 ECG / 人工标注 / gold standard，当前阶段只做工程可编译与基础行为 smoke test，不宣称准确性。

## M2 测试：API compile test

目标：冻结并验证最小 C API 可被外部 C 调用。

覆盖要点：

1. C99 + `-Wall -Wextra -Werror`；
2. `ppg_ibi_context_size() > 0`；
3. `config_default/init/reset/process/version` 可调用；
4. `allow_measure=false` 返回 `NO_EVENT` 且 `ibi_ms=0`；
5. `NULL` 参数返回确定错误状态。

## M3 测试：input validation test

目标：验证逐点输入路径基础行为与异常标记。

覆盖要点：

1. `sample_counter` 递增、`reset()` 后从 1 重新开始；
2. `allow_measure=false` 时进入 `HOLD`，`reject_reason=ALLOW_MEASURE_FALSE`；
3. `allow_measure` 从 false 恢复 true 后进入 `REACQUIRE`；
4. 正常 20ms 间隔不触发 timestamp reject；
5. timestamp 小于预期触发 `TIMESTAMP_GAP`；
6. timestamp 大于预期触发 `SAMPLE_DROP`（或对应 debug flag）；
7. 24-bit signed PPG 边界值/越界触发 `SATURATED`；
8. M3 不返回 `EVENT_READY`，且 `ibi_ms` 保持 0。

## M4 新增测试：signal quality / channel selection test

目标：验证无历史缓存条件下的最小 SQI 与主通道选择行为。

覆盖要点：

1. `allow_measure=false` 时 `selected_channel=255`、`signal_quality=0`、`ibi_ms=0`；
2. `allow_measure=true` 且多路 basic-valid 时按确定性规则选通道（同分取最小 index）；
3. near-saturation 低质量通道与普通有效通道混合时，优先选高质量通道；
4. 全通道 low-quality 且未越界时，触发 `LOW_SIGNAL_QUALITY` 或低质量 debug flag；
5. 任一路达到 24-bit 边界/越界时，触发 `SATURATED` 或 saturated debug flag；
6. M4 仍不返回 `EVENT_READY`，`ibi_ms` 保持 0。

## 约束检查

执行：

- 禁止动态内存关键字扫描（include/src/tests）；
- 禁止输出字段关键字扫描：`hr_bpm` / `rmssd` / `RMSSD`（include/src/tests）；
- public header 变更检查：`git diff -- include/ppg_ibi.h include/ppg_ibi_config.h`。

## 长期计划（保留）

1. 保留示例 CSV smoke test（`tests/fixtures/sample_ppg_20000.csv`）；
2. 在无 gold standard 前，不引入准确性指标（MAE/RMSE/matched beats）；
3. host 侧脚本仅允许 Python 标准库，不引入第三方依赖。

## M5 修复轮 1 补充

- 新增 `tests/test_pulse_detector.c`：覆盖首个候选不出事件、合法 IBI 触发 EVENT_READY、过短 IBI reject、selected channel 切换防伪峰、事件字段一致性。
