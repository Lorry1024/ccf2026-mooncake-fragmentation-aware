# 赛题对齐指标记录

## 仿真来源

- 源码：`repro/topic_aligned_store_scalability_sim.cpp`
- 日志：`logs/topic_aligned_store_scalability_sim_20260706.log`

## 指标表

| 指标 | `free_ratio_first` | `fragmentation_aware` | 解释 |
| --- | ---: | ---: | --- |
| 大对象首选segment可直接容纳 | 0/6 | 6/6 | 新策略首选质量更高 |
| 最终可容纳 | 6/6 | 6/6 | 两者通过fallback最终都能找到可用segment |
| fallback尝试次数 | 11 | 0 | 新策略减少无效尝试 |
| 平均候选segment数量 | 5.00 | 5.00 | 新策略未扩大候选规模 |
| 单次决策耗时 | 121.00ns | 138.93ns | 新策略有小幅计算开销 |
| 新增决策开销 | 不适用 | 17.93ns | 本地CPU仿真结果 |

## 解读

这些指标说明本项目对赛题2中的“Store吞吐稳定性”和“可扩展性”具有直接对应关系。收益来自减少碎片化场景下的失败尝试，而不是来自扩大扫描范围或改变协议。
