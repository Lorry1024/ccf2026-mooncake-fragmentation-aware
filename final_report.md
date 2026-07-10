# 初赛总结报告

## 队伍信息

- 队伍名称：KVCache Forge
- 队长：刘神舟
- 队员：罗荣裕、王英颖、杨俊
- GitHub仓库：`https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`
- GitLink仓库：`https://gitlink.org.cn/liushenzhou/ccf2026-mooncake-fragmentation-aware`

## 项目概述

本项目面向CCF2026开源创新大赛Mooncake赛题2，完成了Mooncake Store碎片感知分配策略设计、实现、测试和提交材料整理。项目核心成果是新增`fragmentation_aware`分配策略，在混合大小KVCache对象场景下，优先选择最大连续空闲区域能够满足当前请求的segment。

## 主要贡献

1. 新增`FragmentationAwareAllocationStrategy`。
2. 新增`AllocationStrategyType::FRAGMENTATION_AWARE`。
3. 支持`--allocation_strategy=fragmentation_aware`启动参数。
4. 保留默认`random`策略，保证兼容和可回滚。
5. 保留preferred segment、excluded segment和best-effort replica语义。
6. 增加碎片化定向单元测试。
7. 将新策略加入benchmark矩阵。
8. 更新Mooncake Store设计和部署文档。
9. 创建Mooncake上游draft PR并通过GitHub Actions。
10. 准备中文技术报告、展示PPT、复现实验和提交压缩包。

## 关键数据

| 指标 | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: |
| 大对象首选segment可直接容纳 | 0/6 | 6/6 |
| fallback尝试次数 | 11 | 0 |
| 平均候选segment数量 | 5.00 | 5.00 |
| 单次决策耗时 | 121.00ns | 138.93ns |
| 新增决策开销 | 不适用 | 17.93ns |

## PR与CI

| 项目 | 内容 |
| --- | --- |
| PR | `https://github.com/kvcache-ai/Mooncake/pull/2797` |
| PR头提交 | `0123fa1 Fix fragmentation-aware allocation test setup` |
| CI结果 | 26个检查成功，1个检查跳过 |

## 技术价值

本项目并不是泛泛提出KVCache优化概念，而是落到Mooncake Store已有分配策略框架中的一个具体问题：总空闲比例无法反映大对象是否能被连续空间容纳。通过引入最大连续空闲区域和连续性评分，新策略在不做全局扫描的前提下改善首选segment质量。

## 当前边界

当前阶段不宣称：

- 完成真实RDMA环境性能测试。
- 完成SGLang HiCache端到端benchmark。
- 完成Mooncake高可用机制重设计。
- 完成生产集群长时间压力测试。

这些边界已经在提交材料中明确说明，避免过度声明。

## 后续计划

1. 补充真实Mooncake Store benchmark，统计吞吐、P50/P99和失败次数。
2. 在具备RDMA条件的机器上验证分布式场景。
3. 将SGLang HiCache接入Mooncake Store后端，测试真实推理负载。
4. 增加长时间混合大小对象churn测试。
5. 根据上游维护者反馈继续完善PR。
