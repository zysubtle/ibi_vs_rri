# AGENTS.md — Codex / Agent 协作规则

## 项目定位

本项目为嵌入式 MCU 端 PPG-IBI 自研算法项目，用于从 PPG 信号中输出逐搏 IBI，供后续 HRV 分析使用。

本项目默认是研究 / 工程验证项目，不作为医疗诊断软件。

## Codex 基本职责

Codex 负责：

1. 按 `docs/10_CODEX_NEXT_TASK.md` 执行当前任务；
2. 修改允许范围内的文件；
3. 运行任务指定的测试命令；
4. 报告修改文件、测试命令、测试结果、限制和未完成事项。

Codex 不负责：

1. push；
2. 创建 PR；
3. 更新 PR；
4. merge / squash merge / rebase merge；
5. 删除远程分支；
6. 改写远程历史。

Owner 自行负责创建分支、push、创建 PR 和合并 PR。

## 必读文件

每次开始任务前，Codex 必须读取：

```text
docs/10_CODEX_NEXT_TASK.md
docs/00_PROJECT_BRIEF.md
docs/04_IO_CONTRACT.md
docs/06_RESOURCE_BUDGET.md
docs/07_TEST_STRATEGY.md
docs/08_RISK_REVIEW.md
```

## 禁止事项

除非 `docs/10_CODEX_NEXT_TASK.md` 明确授权，否则 Codex 不得：

1. 改变公开 API；
2. 改变输入 / 输出字段；
3. 改变采样率；
4. 改变运动门控策略；
5. 改变状态机语义；
6. 引入第三方 PPG / IBI / HR / HRV 算法库；
7. 使用 malloc / calloc / realloc；
8. 使用动态容器或不可控大栈数组；
9. 将算法核心改为 Python、C++ 或其他语言；
10. 将 host 测试脚本当成 MCU 算法实现；
11. 执行远程 Git 操作。

## 允许事项

Owner 已确认：

1. 核心算法语言为 C；
2. 允许 C 标准库；
3. 允许 float，但需记录无 FPU 风险；
4. 允许 CMSIS-DSP，但不得引入第三方 PPG / IBI / HR / HRV 算法库；
5. 允许 Python 标准库作为 host 测试辅助；
6. 禁止 Python 第三方库作为测试依赖，除非 Owner 另行决策。

## 输出摘要要求

每轮任务结束后，Codex 输出摘要至少包含：

1. Summary；
2. Changed files；
3. Test commands；
4. Test results；
5. Known limitations；
6. 是否修改 API；
7. 是否修改输出字段；
8. 是否引入动态内存；
9. 是否引入外部依赖；
10. 是否执行了任何 Git 操作。
