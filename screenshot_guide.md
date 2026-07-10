# GitHub与CI证据截图清单

## 截图通用要求

- 使用桌面浏览器浅色主题，页面缩放80%-100%。
- 建议截图宽度至少1600像素，保存为PNG。
- 只保留与证据有关的页面区域，裁掉浏览器收藏夹、下载栏和大面积空白。
- 截图前隐藏邮箱、通知、访问令牌、内网地址和无关标签页。
- 不修改网页文字，不使用拼接伪造状态；如需标注，只添加简单箭头或矩形框。
- 文件名使用两位序号，例如`01_pr_overview.png`。

## 必须截图

### 1. PR总体状态

页面：`https://github.com/kvcache-ai/Mooncake/pull/2797`

需要保留：PR标题、`Draft`状态、目标分支`kvcache-ai:main`、来源分支`Lorry1024:ccf-fragmentation-aware-allocation`、PR编号2797和提交数量。

建议文件名：`01_pr_overview.png`

报告位置：第4章“实现路径”末尾，用于证明代码已经提交Mooncake上游。

### 2. PR全部检查通过

页面：PR页面底部的合并检查区域。

需要保留：`All checks have passed`、`26 successful checks`、`1 skipped`以及`Review required`。不要把`Review required`裁掉，它表示仍需维护者审批，不代表CI失败。

建议文件名：`02_pr_all_checks_passed.png`

报告位置：第5章“CI验证”。这是最重要的一张CI证据截图。

### 3. GitHub Actions成功任务列表

页面：从PR的Checks进入最终成功的`Build & Test (Linux)`运行。

需要保留：左侧或摘要中的绿色任务列表，至少能看到`Check code format`、`Check Sphinx docs build`、`build (3.10)`、`build (3.12)`、`build-arm64`、`build-flags`等项目。截图顶部应保留workflow名称和成功状态。

建议文件名：`03_actions_success_jobs.png`

报告位置：第5章“CI验证”或“测试修复与CI闭环”。

### 4. allocation_strategy_test通过

页面：最终成功CI运行中的`build (3.10)`或`build (3.12)`，展开`Test (in build env) with coverage`步骤。

需要保留：`allocation_strategy_test`显示`Passed`，以及测试汇总没有失败的区域。可以使用日志搜索框搜索`allocation_strategy_test`快速定位。

建议文件名：`04_allocation_strategy_test_passed.png`

报告位置：第4章“单元测试设计”之后，用于证明新增策略测试进入上游测试集。

### 5. 核心代码差异

页面：PR的`Files changed`。

需要保留：文件路径`mooncake-store/include/allocation_strategy.h`，以及`FragmentationAwareAllocationStrategy`、`can_fit`、`largest_free_region`或排序逻辑附近的代码。不要一次截取过多代码，保证关键行在正常缩放下可读。

建议文件名：`05_core_code_diff.png`

报告位置：第4章“策略类接入”或“核心执行流程”。

### 6. 碎片化单元测试差异

页面：PR的`Files changed`。

需要保留：`mooncake-store/tests/allocation_strategy_test.cpp`文件路径，以及构造`fragmented`和`contiguous`候选、断言新策略选择连续空间候选的代码。

建议文件名：`06_fragmentation_test_diff.png`

报告位置：第4章“单元测试设计”。

### 7. 比赛材料公开仓库

页面：`https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`

需要保留：公开仓库名称、README项目介绍，以及`report/`、`slides/`、`repro/`、`logs/`、`release/`等目录。确保页面能表明仓库是Public。

建议文件名：`07_public_material_repo.png`

报告位置：第6章“评审证据链”或提交材料章节。

## 可选截图

### 8. benchmark接入

在PR的`Files changed`中截图`allocation_strategy_bench.cpp`加入`fragmentation_aware`的部分，证明策略已经进入正式benchmark矩阵。

建议文件名：`08_benchmark_integration.png`

### 9. 文档接入

截图Mooncake设计文档或部署文档中新增的`fragmentation_aware`配置说明，证明策略具有用户入口而不是隐藏实验代码。

建议文件名：`09_docs_integration.png`

### 10. Release与SHA-256

截图比赛仓库Release或`release/`目录，显示最终zip、PDF、PPT和SHA-256校验文件。

建议文件名：`10_release_artifacts.png`

## 推荐选入PDF的组合

PDF不宜放入过多网页截图。建议最终使用四张：PR总体状态、全部检查通过、核心代码差异、公开材料仓库。单元测试和Actions任务列表可放入PPT或视频，避免报告正文被网页界面占满。

## 推荐选入PPT的组合

如果后续增加真实截图，建议将当前PPT第6页拆成两页：第一张左侧放PR总体状态、右侧放全部检查通过；第二张左侧放核心代码差异、右侧放`allocation_strategy_test`通过日志。每张截图下方只保留一句结论，不逐字解释网页内容。
