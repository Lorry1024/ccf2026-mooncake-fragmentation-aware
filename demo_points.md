# 展示视频要点

## 必须讲清楚的三件事

1. 我们选的是Mooncake赛题2，具体切入点是Mooncake Store分配策略。
2. 问题是总空闲比例不能反映最大连续空闲区域，碎片化会导致大对象失败尝试。
3. 新策略`fragmentation_aware`优先选择可直接容纳当前请求的segment，降低fallback压力。

## 建议展示顺序

1. 打开GitHub材料仓库。
2. 打开Mooncake PR#2797，展示CI通过。
3. 打开技术报告PDF，展示架构图和结果表。
4. 打开`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`，展示代码改动文件。
5. 打开`logs/topic_aligned_store_scalability_sim_20260706.log`，展示0/6到6/6、11到0的数据。

## 一句话总结

本作品在不改变Mooncake默认行为的前提下，为Mooncake Store新增碎片感知分配策略，用最大连续空闲区域修正总空闲比例的误判，从而提升混合大小KVCache对象场景下的分配稳定性。
