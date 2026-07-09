# 使用说明

## 启用新策略

在Mooncake master启动时指定：

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

如果同时启用HTTP metadata server：

```bash
mooncake_master \
  --allocation_strategy=fragmentation_aware \
  --enable_http_metadata_server=true \
  --http_metadata_server_port=8080
```

## 适用场景

建议在以下场景启用：

- KVCache对象大小差异较大。
- Store运行时间较长，segment内部出现明显碎片。
- 大对象分配经常失败，但总空闲空间看起来仍然充足。
- 希望降低fallback和重试路径压力。

不建议在以下场景盲目启用：

- 所有对象大小高度一致。
- 更关注极限裸分配吞吐，而不是碎片化下的稳定性。
- 尚未完成生产灰度和监控。

## 应用补丁

在干净的Mooncake工作区中：

```bash
git apply --check /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

回滚：

```bash
git apply -R /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

## 复现实验

```bash
g++ -std=c++17 -O2 repro/fragmentation_aware_sim.cpp -o fragmentation_aware_sim
./fragmentation_aware_sim

g++ -std=c++17 -O2 repro/fragmentation_aware_metrics.cpp -o fragmentation_aware_metrics
./fragmentation_aware_metrics

g++ -std=c++17 -O2 repro/topic_aligned_store_scalability_sim.cpp -o topic_aligned_store_scalability_sim
./topic_aligned_store_scalability_sim
```

对应日志位于：

```text
logs/fragmentation_aware_sim_verify_20260703_0002.log
logs/fragmentation_aware_metrics_verify_20260703_0002.log
logs/topic_aligned_store_scalability_sim_20260706.log
```

## 生产灰度建议

1. 先在测试集群启用`fragmentation_aware`。
2. 对比`random`、`free_ratio_first`和`fragmentation_aware`。
3. 观察分配失败次数、fallback次数、P50/P99延迟、segment碎片率和内存利用率。
4. 如果指标稳定，再逐步扩大范围。
5. 出现异常时将参数切回`random`或`free_ratio_first`即可完成回滚。
