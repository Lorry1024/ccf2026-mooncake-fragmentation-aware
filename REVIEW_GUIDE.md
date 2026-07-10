# 评审阅读指南

## 建议阅读顺序

1. `README.md`：快速了解项目定位、PR状态和主要数据。
2. `report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf`：阅读完整技术报告。
3. `DESIGN.md`：查看设计目标、算法和兼容性。
4. `technical_solution.md`：查看实现路径和策略细节。
5. `EVALUATION.md`：查看测试、仿真和边界说明。
6. `mooncake_fragmentation_aware_pr_2797_0123fa1.patch`：查看精确代码diff。
7. `repro/`和`logs/`：复现实验和验证日志。
8. `slides/Mooncake_FragmentationAware_初赛展示_更新版.pptx`：查看答辩展示材料。

## 快速链接

| 类型 | 链接 |
| --- | --- |
| 比赛材料仓库 | `https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware` |
| Mooncake draft PR | `https://github.com/kvcache-ai/Mooncake/pull/2797` |
| Mooncake fork分支 | `https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation` |

## 当前状态

- PR头提交：`0123fa1 Fix fragmentation-aware allocation test setup`
- GitHub Actions：26个检查成功，1个检查跳过
- PR状态：Draft，等待后续正式评审

## 评审边界

本作品优化的是Mooncake Store分配路径，不宣称已经完成Mooncake高可用协议重设计、RDMA传输优化或真实SGLang HiCache端到端压测。当前证据集中在代码实现、上游CI、分配策略单元测试和确定性仿真。
