# 技术方案：基于连续空闲区域的Mooncake Store分配优化

## 赛题理解

赛题2要求优化Mooncake Store吞吐性能、高可用功能和可扩展性，并关注SGLang HiCache+Mooncake Store性能。Mooncake Store作为KVCache后端时，真正影响性能的不只是网络传输和put/get接口本身，还包括Master为每个对象选择segment的决策质量。

在混合大小KVCache对象场景下，segment内部碎片会让“总空闲空间”与“当前请求是否能分配成功”脱钩。因此，本项目选择从Store分配策略层切入，优化碎片化场景下的首选segment选择。

## 现有策略的不足

`free_ratio_first`使用总空闲比例作为排序依据。该策略适合均衡segment利用率，但在大对象分配时可能出现如下问题：

1. segmentA总空闲空间高，但被切成多个小块。
2. segmentB总空闲空间低一些，但存在足够大的连续空闲区域。
3. 当前请求必须落在一个连续区域中。
4. `free_ratio_first`优先选择segmentA，造成一次失败尝试。
5. 系统随后进入fallback或重试路径，增加分配路径开销。

## 核心思路

新增`fragmentation_aware`策略，在候选segment排序时引入两个信息：

- 当前请求是否可以被最大连续空闲区域直接容纳。
- 空闲空间的连续性比例。

策略仍然沿用有界采样，不做全局遍历，避免随着集群规模增长而引入不可控开销。

## 算法流程

```text
输入：请求大小request_size、副本数replica_count、可用segment集合

1. 优先尝试preferred segment，保持原有语义。
2. 从可用segment中采样有界候选集合。
3. 对每个候选计算total_free、largest_free_region、free_ratio、contiguity_ratio和can_fit。
4. 按can_fit、score、largest_free_region、free_ratio排序。
5. 按排序结果尝试分配。
6. 未满足的副本继续走原有随机best-effort fallback路径。
```

评分公式：

```text
score=0.70*contiguity_ratio+0.30*free_ratio
```

其中`contiguity_ratio`强调空闲空间是否集中，`free_ratio`保留总体空间利用信号。

## 与Mooncake代码的集成

| 层次 | 集成方式 |
| --- | --- |
| 类型系统 | 新增`AllocationStrategyType::FRAGMENTATION_AWARE` |
| 策略工厂 | `CreateAllocationStrategy`返回`FragmentationAwareAllocationStrategy` |
| 配置解析 | `master_config.h`识别`fragmentation_aware`字符串 |
| 启动参数 | `master.cpp`帮助信息加入新取值 |
| 测试 | 扩展参数化测试，并新增碎片化定向测试 |
| benchmark | 将新策略加入分配策略矩阵 |
| 文档 | 更新Mooncake Store设计和部署说明 |

## 测试构造

关键单元测试构造两个segment：

- `fragmented`：总空闲空间更高，但最大连续空闲区域小于请求大小。
- `contiguous`：总空闲空间较低，但最大连续空闲区域可以容纳请求。

预期行为：

```text
fragmentation_aware选择contiguous
```

同时测试preferred segment语义，确保用户指定的优先segment仍被保留。

## 评估结果

专题对齐仿真使用5个segment和6个不同大小请求，模拟碎片化和连续空闲区域并存的Store场景。

| 指标 | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: |
| 首选segment可直接容纳 | 0/6 | 6/6 |
| 最终可容纳 | 6/6 | 6/6 |
| fallback尝试次数 | 11 | 0 |
| 平均候选segment数量 | 5.00 | 5.00 |
| 单次决策耗时 | 121.00ns | 138.93ns |
| 额外决策开销 | 不适用 | 17.93ns |

该结果说明，新策略在不扩大候选规模的情况下改善了首选分配质量。额外开销为纳秒级，本地仿真中约为17.93ns/请求。

## 边界说明

当前阶段已经完成代码实现、单元测试、benchmark接入、文档更新和上游PR CI验证。尚未宣称完成以下内容：

- 真实RDMA环境benchmark。
- 真实SGLang HiCache端到端压测。
- Mooncake高可用协议重设计。
- 生产集群长时间压测。

这些内容适合作为决赛阶段或后续PR迭代的工作。
