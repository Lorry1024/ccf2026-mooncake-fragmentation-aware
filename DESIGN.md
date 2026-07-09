# 设计文档：Mooncake Store碎片感知分配策略

## 背景

Mooncake Store负责为KVCache对象提供分布式存储能力。现有分配策略中，`random`追求简单和低开销，`free_ratio_first`通过总空闲比例改善segment之间的空间利用均衡。但是，在长时间运行的推理服务中，KVCache对象大小并不固定，分配和释放会让同一个segment内部出现许多小空洞。

当请求对象较大时，仅观察总空闲空间会产生误判：某个segment总空闲空间很多，但最大连续空闲区域不足，实际无法容纳当前对象；另一个segment总空闲空间稍少，但拥有足够大的连续空闲区域，反而能够一次分配成功。

## 问题定义

| segment | 总空闲空间 | 最大连续空闲区域 | 10MiB请求能否放入 |
| --- | ---: | ---: | --- |
| fragmented | 16MiB | 8MiB | 不能 |
| contiguous | 12MiB | 12MiB | 可以 |

`free_ratio_first`会选择总空闲比例更高的`fragmented`，但该选择会失败。`fragmentation_aware`希望优先选择`contiguous`，因为它能够直接满足当前请求。

## 设计目标

1. 不改变默认策略，保持现有部署兼容。
2. 不引入全局扫描，保留现有有界采样形态。
3. 优先选择最大连续空闲区域能够满足请求的segment。
4. 保留preferred segment、excluded segment和best-effort replica语义。
5. 将策略作为独立选项接入，便于灰度、回滚和对比。

## 新策略

新策略通过如下参数启用：

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

对每个候选segment计算以下信息：

```text
total_free=sum(capacity-used)
largest_free_region=max(allocator.getLargestFreeRegion())
free_ratio=total_free/total_capacity
contiguity_ratio=largest_free_region/total_free
can_fit=largest_free_region>=request_size
score=0.70*contiguity_ratio+0.30*free_ratio
```

排序规则为：

1. `can_fit=true`的候选优先。
2. `score`更高的候选优先。
3. `largest_free_region`更大的候选优先。
4. `free_ratio`更高的候选优先。
5. 保留稳定tie-break，避免非确定性排序。

如果排序后的候选segment仍无法满足所有副本，新策略回退到原有随机best-effort路径。

## 代码改动范围

| 文件 | 改动 |
| --- | --- |
| `mooncake-store/include/allocation_strategy.h` | 新增`FragmentationAwareAllocationStrategy`和候选评分逻辑 |
| `mooncake-store/include/types.h` | 新增`AllocationStrategyType::FRAGMENTATION_AWARE` |
| `mooncake-store/include/master_config.h` | 支持解析`fragmentation_aware`参数 |
| `mooncake-store/src/master.cpp` | 更新`--allocation_strategy`帮助信息 |
| `mooncake-store/tests/allocation_strategy_test.cpp` | 增加碎片化选择和preferred segment测试 |
| `mooncake-store/benchmarks/allocation_strategy_bench.cpp` | 将新策略加入benchmark矩阵 |
| `docs/source/design/mooncake-store.md` | 补充策略说明和适用场景 |
| `docs/source/deployment/mooncake-store-deployment-guide.md` | 补充部署参数说明 |
| `.github/workflows/ci.yml` | 为Go store binding测试前置磁盘清理，避免runner磁盘不足导致无关失败 |

## 兼容性

- 默认策略仍为`random`。
- 用户必须显式指定`--allocation_strategy=fragmentation_aware`才会启用新策略。
- 已有客户端接口、存储协议和metadata路径不变。
- 对不能提供精确最大连续空闲区域的allocator，策略退化为基于可用空闲空间的保守处理。
- 当没有候选segment能够满足请求时，新策略不会制造容量，仍遵循原有fallback行为。

## 预期收益

该策略不会直接提升所有场景下的裸分配吞吐，但能减少碎片化场景下的无效首选分配尝试。在长时间运行、对象大小混合、segment内部空闲区域被切碎的KVCache Store中，这种减少失败尝试的能力有助于稳定分配路径延迟，并降低fallback、重试或后续淘汰路径压力。
