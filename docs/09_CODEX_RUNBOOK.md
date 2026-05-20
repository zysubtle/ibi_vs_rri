# Codex Runbook v0.1

## 基本指令

Owner 给 Codex 的默认一句话指令：

```text
请读取 docs/10_CODEX_NEXT_TASK.md，并严格执行。请不要 push、不要创建 PR、不要 merge；完成后报告修改文件和测试结果。
```

## Codex 每轮开始前

Codex 应读取：

1. `AGENTS.md`；
2. `docs/10_CODEX_NEXT_TASK.md`；
3. 当前任务引用的其他 docs。

## Codex 每轮结束后

Codex 应报告：

1. Summary；
2. Changed files；
3. Test commands；
4. Test results；
5. Known limitations；
6. 是否修改 API；
7. 是否修改 IO Contract；
8. 是否使用 malloc / calloc / realloc；
9. 是否引入外部依赖；
10. 是否执行 Git 操作。

## 禁止远程 Git 操作

Codex 不得：

```text
push
创建 PR
更新 PR
merge
rebase
删除远程分支
改写 main / master 历史
```

## 示例数据规则

如果任务使用示例数据，必须引用仓库相对路径：

```text
tests/fixtures/sample_ppg_20000.csv
```

不得引用：

```text
ChatGPT 附件
ChatGPT Sources
用户本地绝对路径
临时下载目录
```

## 审查材料

Owner 可向 Architect 提供：

1. PR 链接；
2. Codex 摘要；
3. 项目 zip；
4. diff + 测试日志。

PR 由 Owner 自行创建。
