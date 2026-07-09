# 测试说明

## 测试目标

测试目标是验证`fragmentation_aware`策略是否满足以下要求：

1. 能在碎片化场景下优先选择拥有足够连续空闲区域的segment。
2. 不破坏已有`random`和`free_ratio_first`策略。
3. 不破坏preferred segment语义。
4. 能通过上游Mooncake GitHub Actions。
5. 能通过独立仿真复现关键指标。

## 上游CI结果

Mooncake draft PR：

```text
https://github.com/kvcache-ai/Mooncake/pull/2797
```

当前头提交：

```text
0123fa1 Fix fragmentation-aware allocation test setup
```

CI结果：

```text
26个检查成功，1个检查跳过
```

这说明当前PR分支已经通过上游Linux构建、格式检查、文档构建和相关测试。

## 新增单元测试

测试名称：

```text
FragmentationAwarePrefersContiguousSpaceOverTotalFreeSpace
```

构造方式：

- `fragmented` segment容量为40MiB，先分配5个8MiB块，然后释放中间块，形成总空闲16MiB但最大连续空闲8MiB的状态。
- `contiguous` segment容量为40MiB，保留一个约12MiB的连续空闲区域。
- 请求大小为10MiB。

预期结果：

```text
fragmentation_aware选择contiguous
```

另一个测试验证preferred segment语义，确保显式preferred segment不会因为新评分逻辑被错误跳过。

## 专题对齐仿真

运行目标：

```text
repro/topic_aligned_store_scalability_sim.cpp
```

结果日志：

```text
logs/topic_aligned_store_scalability_sim_20260706.log
```

关键指标：

| 指标 | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: |
| 首选segment可直接容纳 | 0/6 | 6/6 |
| fallback尝试次数 | 11 | 0 |
| 平均候选segment数量 | 5.00 | 5.00 |
| 单次决策耗时 | 121.00ns | 138.93ns |

## 本地复现命令

在Linux或WSL环境中：

```bash
g++ -std=c++17 -O2 repro/fragmentation_aware_sim.cpp -o fragmentation_aware_sim
./fragmentation_aware_sim

g++ -std=c++17 -O2 repro/fragmentation_aware_metrics.cpp -o fragmentation_aware_metrics
./fragmentation_aware_metrics

g++ -std=c++17 -O2 repro/topic_aligned_store_scalability_sim.cpp -o topic_aligned_store_scalability_sim
./topic_aligned_store_scalability_sim
```

预期输出应包含：

```text
fragmentation_aware_choice=contiguous can_fit=yes
PASS fragmentation_aware_metrics
PASS topic_aligned_store_scalability_sim
```

## 历史本地构建边界

早期在Windows挂载目录和WSL Ubuntu20.04环境中尝试完整构建Mooncake Store时，遇到过与本项目逻辑无关的环境问题，包括旧版`libibverbs`、旧版libstdc++的C++20接口缺失以及本地内存压力。后续以上游GitHub Actions作为更可靠的完整验证来源。

## 当前测试结论

当前测试链路已经覆盖策略正确性、边界场景、PR可构建性和文档格式。仍未覆盖真实RDMA硬件环境和SGLang HiCache端到端负载，这部分应在具备硬件和时间条件后补充。
