# M2 里程碑启动卡

## 当前里程碑

M2：IO Contract、MCU C API 与资源约束冻结

## 本阶段目的

把 M1 中仍为草案的输入 / 输出字段、C API、状态枚举、reject reason、context 分配方式和资源约束冻结到可执行的仓库文件中。

## 为什么现在做

M1 只生成项目文档包，没有源码骨架。进入算法实现前，必须先冻结最小 API 与资源边界，避免后续 M3–M6 在接口、字段和状态语义上反复返工。

## 本阶段产出

1. `include/ppg_ibi.h` 最小公开 API；
2. `include/ppg_ibi_config.h` 配置常量；
3. `src/ppg_ibi.c` 占位实现；
4. `tests/test_api_compile.c` 编译测试；
5. 更新 `docs/04_IO_CONTRACT.md`、`docs/06_RESOURCE_BUDGET.md` 至 v0.2。

## 本阶段不做

1. 不做滤波；
2. 不做 SQI；
3. 不做主通道选择；
4. 不做脉搏峰检测；
5. 不输出真实 IBI。

## 验收标准

1. `make test` 通过；
2. 公开 API 可被 C 测试文件 include 并调用；
3. 未使用 `malloc/calloc/realloc`；
4. 未引入第三方 PPG / IBI / HR / HRV 算法库；
5. 文档与代码中的输入 / 输出字段、状态、reject reason 一致。

## 是否需要 Owner 决策

无。Owner 已确认进入 M2。若 Codex 认为需要改变 API、输出字段、资源约束或远程 Git 策略，必须暂停并报告为 S0。

## Owner 下一步

将本 ZIP 解压覆盖到仓库根目录，然后把以下一句话交给 Codex：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## docs/10_CODEX_NEXT_TASK.md

本包已提供完整替换版本。
