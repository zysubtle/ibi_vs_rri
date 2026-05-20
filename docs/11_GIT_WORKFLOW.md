# Git Workflow v0.1

## 职责边界

Owner 负责：

1. 创建分支；
2. 将文档包或 Codex 产物放入仓库；
3. push；
4. 创建 PR；
5. 合并 PR。

Codex 负责：

1. 本地修改；
2. 本地测试；
3. 报告结果。

Codex 不负责：

1. push；
2. 创建 PR；
3. 更新 PR；
4. merge；
5. rebase；
6. 删除远程分支。

## 建议分支命名

```text
feature/m1-project-docs
feature/m2-api-contract
feature/m3-input-validation
```

## 建议 Owner 操作

M1 文档包可由 Owner 手动解压到仓库后创建 PR。

示例：

```bash
git checkout -b feature/m1-project-docs
# 解压 M1 ZIP 到仓库根目录
git status
git add AGENTS.md docs tests/fixtures/README.md README.md
git commit -m "M1: add MCU PPG-IBI project documentation package"
git push origin feature/m1-project-docs
# Owner 自行创建 PR
```

以上命令仅供 Owner 参考，不应要求 Codex 执行远程操作。
