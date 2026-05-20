# MCU PPG-IBI 自研算法项目启动包 M1

本包是 M1：项目启动包与文档骨架。

## 当前状态

- Project Brief v0.1：Owner 已确认。
- M1 范围：只生成项目文档包，不生成源码骨架、不实现算法、不生成测试代码。
- Codex 权限：只负责本地编码 / 测试 / 报告；不得 push、不得创建 PR、不得 merge。
- 示例数据：Owner 声明 `示例数据_20000.csv` 可入仓库，但本包未包含实际 CSV 文件。

## 建议放置方式

将本 ZIP 解压到项目仓库根目录，使结构类似：

```text
AGENTS.md
docs/
  00_PROJECT_BRIEF.md
  01_DECISION_LOG.md
  02_MILESTONE_PLAN.md
  03_ALGORITHM_SCOPE.md
  04_IO_CONTRACT.md
  05_MCU_ALGORITHM_STRATEGY.md
  06_RESOURCE_BUDGET.md
  07_TEST_STRATEGY.md
  08_RISK_REVIEW.md
  09_CODEX_RUNBOOK.md
  10_CODEX_NEXT_TASK.md
  11_GIT_WORKFLOW.md
tests/
  fixtures/
    README.md
```

## 示例数据

请由 Owner 将实际 CSV 放入：

```text
tests/fixtures/sample_ppg_20000.csv
```

原始文件名可记录为：

```text
示例数据_20000.csv
```

Codex 测试任务只能引用仓库内相对路径，不得引用 ChatGPT 附件、Sources 或本地临时路径。

## 下一步

Owner 将本包解压到仓库后，可选择：

1. 上传关键 Markdown 到 ChatGPT Project Sources，作为 Architect 的参考上下文；
2. 将示例 CSV 放入 `tests/fixtures/sample_ppg_20000.csv`；
3. 回复 Architect：`M1 已放入仓库，进入 M2`。
