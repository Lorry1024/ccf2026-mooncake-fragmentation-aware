# 5分钟展示视频讲稿

## 0:00-0:30 项目介绍

大家好，我们的作品是“Mooncake Store碎片感知分配策略优化”，对应CCF2026开源创新大赛Mooncake赛题2。赛题2关注Mooncake Store吞吐性能、高可用、可扩展性以及SGLang HiCache+Mooncake Store性能。我们的切入点是Store分配路径：在混合大小KVCache对象场景下，优化Master选择segment的策略。

## 0:30-1:20 问题背景

Mooncake Store已有`random`和`free_ratio_first`等策略。`free_ratio_first`会优先选择总空闲比例高的segment，但总空闲空间并不等于当前请求一定能放进去。长时间运行后，KVCache对象反复分配和释放，会把空闲空间切成多个小块。一个segment可能有很多总空闲空间，但最大连续空闲区域不足，导致大对象分配失败。

我们的目标是让Master优先选择“能够直接容纳当前对象”的segment，而不是只看总空闲比例。

## 1:20-2:20 方案设计

我们新增了`fragmentation_aware`策略。它仍然保持Mooncake原有的有界候选采样，不做全局扫描。对每个候选segment计算总空闲空间、最大连续空闲区域、空闲比例、连续性比例和是否能容纳当前请求。

排序规则是：能容纳当前请求的segment优先，然后比较碎片感知评分，再比较最大连续空闲区域和总空闲比例。如果候选不能满足所有副本，仍然回退到原有best-effort fallback路径。

## 2:20-3:20 实现内容

代码改动主要包括：

- 新增`AllocationStrategyType::FRAGMENTATION_AWARE`。
- 实现`FragmentationAwareAllocationStrategy`。
- 支持`--allocation_strategy=fragmentation_aware`参数。
- 增加碎片化场景单元测试。
- 将新策略加入benchmark矩阵。
- 更新Mooncake Store设计文档和部署文档。

我们还创建了上游Mooncake draft PR，PR编号是`kvcache-ai/Mooncake#2797`。

## 3:20-4:20 实验结果

在确定性碎片化仿真中，`free_ratio_first`在6个大对象请求里首选segment可直接容纳的次数是0/6，而`fragmentation_aware`是6/6。fallback尝试次数从11降到0。两个策略的平均候选segment数量都是5.00，说明新策略没有扩大采样规模。单次决策耗时从121.00ns增加到138.93ns，额外开销约17.93ns。

这个结果说明，新策略用很小的决策开销换来了更稳定的首选分配结果。

## 4:20-5:00 当前状态和边界

当前PR头提交是`0123fa1`，GitHub Actions已经通过，结果是26个检查成功、1个检查跳过。我们的提交材料包括源码PR、patch、中文技术报告、PPT、复现实验源码和日志。

需要说明的是，当前阶段我们不宣称完成真实RDMA环境测试或SGLang HiCache端到端benchmark。这些是后续决赛阶段可以继续补充的工作。

视频结束时展示仓库链接、PR链接和技术报告即可。
