# 赛题对齐报告

## 对齐结论

本项目与Mooncake赛题2对齐，重点解决Mooncake Store在混合大小KVCache对象场景下的分配稳定性问题。项目通过新增`fragmentation_aware`策略，将最大连续空闲区域纳入segment选择过程，从而减少碎片化下的失败尝试。

## 为什么属于赛题2

赛题2强调Mooncake Store吞吐性能、高可用、可扩展性和SGLang HiCache+Mooncake Store性能。本项目虽然没有直接改写SGLang HiCache，但优化的是Mooncake Store后端的分配路径，而该路径会影响KVCache对象写入和复用时的稳定性。

## 对齐证据

| 维度 | 证据 |
| --- | --- |
| Store路径 | 改动集中在`mooncake-store`目录 |
| 性能稳定性 | 专题仿真中fallback从11降到0 |
| 可扩展性 | 平均候选segment数量保持5.00 |
| 兼容性 | 默认策略不变，新策略显式开启 |
| 开源规范 | 已创建Mooncake上游PR并通过CI |

## 评审建议

建议评委重点查看：

1. `report/CCF2026_Mooncake_FragmentationAware_Technical_Report.pdf`
2. `mooncake_fragmentation_aware_pr_2797_0123fa1.patch`
3. Mooncake PR#2797
4. `logs/topic_aligned_store_scalability_sim_20260706.log`

## 边界

本项目当前是初赛阶段分配策略优化，不是完整生产级Store压测报告。真实RDMA和SGLang HiCache端到端测试应作为后续工作补充。
