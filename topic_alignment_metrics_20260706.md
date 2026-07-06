# Topic Alignment Metrics - 2026-07-06

Scope: 对齐官方赛题2, Mooncake Store fragmentation-aware allocation for Store scalability/performance stability.

Source log:

`logs/topic_aligned_store_scalability_sim_20260706.log`

Run command:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=c++17 -O2 repro/topic_aligned_store_scalability_sim.cpp -o topic_aligned_store_scalability_sim_20260706 && ./topic_aligned_store_scalability_sim_20260706 > logs/topic_aligned_store_scalability_sim_20260706.log 2>&1"
```

## Summary Metrics

| Metric | free_ratio_first | fragmentation_aware | Competition value | Risk boundary |
|---|---:|---:|---|---|
| Large-object primary fit success | 0/6 | 6/6 | Fewer avoidable failed first allocation attempts in fragmented Store pools. | Deterministic simulation only. |
| Eventual fit success with fallback scan | 6/6 | 6/6 | Shows both can eventually find capacity in this model, but the new strategy reaches it earlier. | Does not model real allocator locks, network, or eviction. |
| Fallback attempts | 11 | 0 | Lower fallback pressure can support throughput stability under high concurrency. | Not a measured Mooncake QPS benchmark. |
| Average candidates scored | 5.00 | 5.00 | Bounded sampling preserves scalable decision scope. | Candidate set is synthetic and fixed at 5. |
| Decision time per request | 121.00 ns | 138.93 ns | The local scoring overhead is small in this CPU-only model. | Microbenchmark is local and not representative of production. |
| Extra decision time | n/a | 17.93 ns | Quantifies candidate scoring overhead instead of hiding it. | Depends on CPU, compiler, and synthetic data size. |

## Per-Request Metrics

| Request MiB | free_ratio choice | free_ratio primary fit | free_ratio attempts | free_ratio fallback attempts | fragmentation-aware choice | fragmentation-aware primary fit | fragmentation-aware attempts | fragmentation-aware fallback attempts | Candidates scored |
|---:|---|---|---:|---:|---|---|---:|---:|---:|
| 20 | fragmented_high_free_a | no | 3 | 2 | contiguous_large_a | yes | 1 | 0 | 5 |
| 18 | fragmented_high_free_a | no | 3 | 2 | contiguous_large_a | yes | 1 | 0 | 5 |
| 12 | fragmented_high_free_a | no | 3 | 2 | contiguous_large_a | yes | 1 | 0 | 5 |
| 10 | fragmented_high_free_a | no | 3 | 2 | contiguous_large_a | yes | 1 | 0 | 5 |
| 8 | fragmented_high_free_a | no | 3 | 2 | contiguous_large_a | yes | 1 | 0 | 5 |
| 6 | fragmented_high_free_a | no | 2 | 1 | contiguous_large_a | yes | 1 | 0 | 5 |

## Interpretation

This simulation makes the official topic-2 alignment explicit:

- Throughput stability: the new ranking avoids choosing fragmented high-free segments that cannot satisfy the current large object.
- Scalability: the candidate sample size remains bounded and equal to the baseline strategy in this model.
- SGLang HiCache relevance: HiCache uses Mooncake Store as a KVCache storage backend; successful large-object placement is relevant to backend stability, but this run is not a real SGLang benchmark.
- High availability: the contribution preserves existing best-effort fallback behavior but does not add a new HA subsystem.
