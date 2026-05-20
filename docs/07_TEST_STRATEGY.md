# Test Strategy v0.2

## 当前定位

无 ECG / 人工标注 / gold standard，当前阶段只做工程可编译与基础行为 smoke test，不宣称准确性。

## M2 新增测试：API compile test

目标：冻结并验证最小 C API 可被外部 C 调用。

`make test` 覆盖：

1. C99 + `-Wall -Wextra -Werror` 编译 `tests/test_api_compile.c` 与 `src/ppg_ibi.c`；
2. 验证 `ppg_ibi_context_size() > 0`；
3. 验证 `config_default/init/reset/process/version` 可调用；
4. 验证 `allow_measure=false` 时返回 `NO_EVENT` 且不产生有效 IBI（`ibi_ms=0`）；
5. 验证 `NULL` 参数返回确定错误状态。

## 约束检查

执行：

- 禁止动态内存关键字扫描（include/src/tests）；
- 禁止输出字段关键字扫描：`hr_bpm` / `rmssd` / `RMSSD`（include/src/tests）。

## 非目标

M2 不做：

1. 真实峰值检测正确性；
2. IBI 准确性指标（MAE/RMSE/matched beats）；
3. ECG 对齐评估。
