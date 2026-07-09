# 评估报告

## 评估范围

本报告评估`fragmentation_aware`相对于`free_ratio_first`在碎片化、混合大小KVCache对象场景下的分配决策质量。当前证据包括上游PR CI、确定性仿真、扩展边界场景仿真和专题对齐仿真。

需要明确的是，当前结果不能等价为完整Mooncake Store生产benchmark，也不能等价为SGLang HiCache端到端吞吐结论。它证明的是分配策略层面的目标问题和优化效果。

## 上游PR CI

| 项目 | 内容 |
| --- | --- |
| PR | `https://github.com/kvcache-ai/Mooncake/pull/2797` |
| 当前头提交 | `0123fa1 Fix fragmentation-aware allocation test setup` |
| CI结果 | 26个检查成功，1个检查跳过 |
| 结论 | 上游构建和测试工作流已通过 |

## 最小碎片化复现

源码：`repro/fragmentation_aware_sim.cpp`

| segment | 总空闲空间 | 最大连续空闲区域 | 请求大小 | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: | ---: | --- | --- |
| `fragmented` | 16MiB | 8MiB | 10MiB | 优先选择但不能放入 | 不作为首选 |
| `contiguous` | 12MiB | 12MiB | 10MiB | 因总空闲比例低被排后 | 优先选择且可放入 |

关键输出：

```text
free_ratio_first_choice=fragmented can_fit=no
fragmentation_aware_choice=contiguous can_fit=yes
```

## 扩展边界场景

源码：`repro/fragmentation_aware_metrics.cpp`

| 场景 | 目的 |
| --- | --- |
| `mixed_10m` | 混合大小对象下的典型大对象选择 |
| `boundary_8m` | 请求等于碎片块边界 |
| `boundary_12m` | 请求超过碎片块但可被连续块容纳 |
| `no_fit_20m` | 所有候选都无法容纳时的边界行为 |
| `zero_free_guard` | 空闲空间为零时的保护逻辑 |

结果：

```text
PASS fragmentation_aware_metrics: validated 5 deterministic ranking and boundary scenarios
```

## 赛题对齐仿真

源码：`repro/topic_aligned_store_scalability_sim.cpp`

日志：`logs/topic_aligned_store_scalability_sim_20260706.log`

| 指标 | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: |
| 大对象首选segment可直接容纳 | 0/6 | 6/6 |
| 最终可容纳 | 6/6 | 6/6 |
| fallback尝试次数 | 11 | 0 |
| 平均候选segment数量 | 5.00 | 5.00 |
| 单次决策耗时 | 121.00ns | 138.93ns |
| 新增决策开销 | 不适用 | 17.93ns |

解释：

- 两个策略最终都能在fallback扫描后找到可用segment。
- 差异在于首选决策质量：`fragmentation_aware`首次选择就能命中可容纳segment。
- 候选数量保持一致，说明新策略没有扩大采样规模。
- 新增开销很小，主要来自读取最大连续空闲区域和计算评分。

## 与赛题要求的对应

| 赛题要求 | 当前完成情况 | 证据 |
| --- | --- | --- |
| Mooncake Store吞吐稳定性 | 通过减少失败尝试降低分配路径压力 | 专题仿真fallback从11降到0 |
| 可扩展性 | 保持有界候选采样，不做全局扫描 | 平均候选segment仍为5.00 |
| 高可用语义 | 保留preferred/excluded和best-effort replica行为 | 单元测试和代码设计 |
| SGLang HiCache相关性 | 作用于Mooncake Store后端分配路径 | 方案说明 |
| 测试 | 上游CI通过，仿真通过 | PR#2797和`logs/` |
| benchmark | 已加入benchmark矩阵，当前有分配路径仿真 | patch和仿真日志 |

## 当前不足

| 项目 | 状态 | 后续建议 |
| --- | --- | --- |
| 真实Mooncake Store吞吐压测 | 未完成 | 使用`allocation_strategy_bench`补充P50/P99和吞吐指标 |
| RDMA环境验证 | 未完成 | 在具备RDMA网卡的集群上执行端到端测试 |
| SGLang HiCache端到端测试 | 未完成 | 接入真实推理负载并统计命中率和后端IO |
| 长时间压力测试 | 未完成 | 构造混合大小对象churn，观察碎片率和失败次数 |

## 结论

当前证据足以证明本项目解决了Mooncake Store分配路径中的一个具体碎片化问题：在总空闲空间排序产生误导时，新策略能够优先选择可直接容纳请求的segment。该方案具有低侵入、可回滚、默认不启用和易于后续benchmark扩展的特点。
