# docs/10_CODEX_NEXT_TASK.md

## 当前状态

当前没有可执行 Codex 编码任务。

原因：M1 的范围是由 Architect 生成正式项目文档包；Owner 需要先将本包解压到仓库根目录，并确认是否进入 M2。

## Owner 下一步

1. 将 M1 ZIP 解压到仓库根目录；
2. 将示例 CSV 放入：

```text
tests/fixtures/sample_ppg_20000.csv
```

3. 如需将关键文档同步给 ChatGPT，可上传 Markdown 到 Project Sources；
4. 回复 Architect：

```text
M1 已放入仓库，进入 M2
```

## 如果 Owner 明确要求 Codex 检查 M1 包

Codex 只允许做文档存在性检查，不得写算法代码。

允许检查：

1. `AGENTS.md` 是否存在；
2. `docs/00_PROJECT_BRIEF.md` 是否存在；
3. `docs/04_IO_CONTRACT.md` 是否存在；
4. `docs/10_CODEX_NEXT_TASK.md` 是否存在；
5. `tests/fixtures/README.md` 是否存在。

禁止：

1. 创建源码；
2. 修改 API；
3. 实现算法；
4. 添加第三方依赖；
5. push；
6. 创建 PR；
7. merge。

## M2 任务草案

以下仅为草案，Owner 未确认进入 M2 前，Codex 不得执行。

M2 建议任务：冻结 IO Contract、MCU C API 与资源约束。

M2 可能产出：

1. `include/ppg_ibi.h` 最小 API 草案；
2. `include/ppg_ibi_config.h` 配置草案；
3. `src/ppg_ibi.c` 占位实现；
4. `tests/test_api_compile.c` 编译测试；
5. 更新 `docs/04_IO_CONTRACT.md` 到 v0.2；
6. 更新 `docs/06_RESOURCE_BUDGET.md` 到 v0.2。

M2 必须由 Owner 明确确认后再生成正式任务文件。
