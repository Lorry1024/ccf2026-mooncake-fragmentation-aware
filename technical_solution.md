# Technical Solution

## Official Topic-2 Positioning

This solution is aligned to official Mooncake `track2_2026Mooncake` **赛题2：优化 Mooncake Store 吞吐性能、高可用功能和可扩展性，优化 SGLang HiCache + Mooncake Store 性能**.

The corrected technical narrative is:

> Mooncake Store fragmentation-aware allocation for Store scalability/performance stability.

The contribution is intentionally scoped to Mooncake Store's allocation strategy layer. It does not claim to redesign SGLang, HiCache, RDMA transport, or Mooncake high-availability protocols. Instead, it improves a Store backend decision that can affect mixed-size KVCache placement: choose a segment whose largest contiguous free region can satisfy the request before ranking only by aggregate free bytes.

Topic-2 mapping:

| Topic-2 concern | How this patch maps to it | Current evidence |
|---|---|---|
| Store throughput stability | Avoids primary selection of fragmented high-free segments that cannot fit the object, reducing retry/fallback work in the allocation path. | `logs/topic_aligned_store_scalability_sim_20260706.log`. |
| Store scalability | Keeps bounded sampled candidate ranking; no full-cluster scan is introduced. | Candidate count remains 5.00 average in the 2026-07-06 simulation. |
| High availability | Preserves preferred/excluded segment semantics and best-effort replica fallback; no new HA subsystem is claimed. | Patch design and rollback notes. |
| SGLang HiCache + Store performance | Relevant to the Store backend used for KVCache objects; stable large-object placement can support backend behavior. | Local simulation only; no real SGLang HiCache benchmark is claimed. |

## Background

Mooncake Store already provides `random` and `free_ratio_first` allocation strategies. `free_ratio_first` improves cluster utilization balance by sampling candidate segments and ranking them by free-space ratio.

For mixed-size KV cache workloads, aggregate free ratio can be misleading. A segment may have high total free space split across small holes, while another segment with slightly less total free space may have a contiguous region large enough for the current request.

## Design

`fragmentation_aware` extends the existing sampled-selection design:

1. Try preferred segments first, preserving existing behavior.
2. Sample `min(6 * remaining_replicas, total_segments)` candidate segments.
3. For each candidate, compute:
   - total capacity.
   - total free bytes.
   - largest free region across its allocators.
   - whether the largest region can fit the requested slice.
4. Sort candidates by:
   - fit-capable segments first.
   - blended score: `0.70 * contiguity_ratio + 0.30 * free_ratio`.
   - largest free region as a tie-breaker.
5. Attempt allocation in ranked order.
6. Fall back to random allocation for remaining replicas.

## Expected Benefit

The strategy avoids selecting segments that look healthy by total free space but cannot satisfy large allocations due to fragmentation. This should reduce failed allocation attempts under mixed-size object churn and improve usable capacity for KV cache workloads.

For official赛题2, this expected benefit should be read as Store allocation-path stability rather than a measured end-to-end QPS claim. The 2026-07-06 deterministic simulation reports:

- `free_ratio_first` primary fit success: 0/6.
- `fragmentation_aware` primary fit success: 6/6.
- `free_ratio_first` fallback attempts: 11.
- `fragmentation_aware` fallback attempts: 0.
- Average candidates scored: 5.00 for both strategies.
- Measured local scoring overhead: about 17.93 ns/request in the synthetic CPU-only model.

## Scope Control

The implementation intentionally avoids changing allocator internals, protocol behavior, or client APIs. It adds one strategy behind an explicit master flag, so existing deployments keep the `random` default unless they opt in.

## Local Build Compatibility

During validation on Ubuntu 20.04, several unrelated build-environment issues blocked a direct Store test binary. The patch includes scoped compatibility fixes:

- `USE_RDMA=OFF` now excludes RDMA transport objects and RPC RDMA initialization paths for TCP-only builds.
- Older `libibverbs` is detected and yalantinglibs is configured to avoid `ibv_query_gid_ex` paths.
- `MasterAdminServer` uses `std::binary_semaphore` when available and a condition-variable fallback on older libstdc++.
- Yalantinglibs source include roots are added explicitly for builds that use the vendored headers.

These changes are build-scope support for reproducibility; they do not change the allocation strategy semantics.

## Nightly Strengthening: 2026-07-03

The current preferred patch is now:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

It corresponds to Mooncake draft PR `https://github.com/kvcache-ai/Mooncake/pull/2797`
at head commit `0123fa1`. The older `mooncake_fragmentation_aware.patch` and
`mooncake_fragmentation_aware_pr_ready_20260703.patch` are retained for
traceability, but the `0123fa1` patch is the current review artifact.

Current PR CI status: 26 successful checks, 1 skipped check.

### Failure Mode

`free_ratio_first` ranks sampled segments by aggregate free-space ratio. This is insufficient when a large KV cache object must fit inside one allocator's contiguous free region:

- Segment A can have higher total free bytes, but split into small holes.
- Segment B can have lower total free bytes, but one contiguous region large enough for the request.
- Ranking A first causes a failed allocation attempt and pushes the system into retry/fallback/eviction paths even though B could have satisfied the request immediately.

### Fragmentation-Aware Score

For each sampled candidate:

- `total_free = sum(capacity - used)`.
- `largest_free_region = max(allocator.getLargestFreeRegion())`.
- `free_ratio = total_free / total_capacity`.
- `contiguity_ratio = largest_free_region / total_free`.
- `can_fit = largest_free_region >= request_size`.
- `fa_score = 0.70 * contiguity_ratio + 0.30 * free_ratio`.

Sort order:

1. `can_fit` candidates first.
2. Higher `fa_score`.
3. Higher `largest_free_region`.
4. Higher `free_ratio`.
5. Stable candidate-index tie break.

### Scope and Boundaries

- Default behavior is unchanged because the new strategy is opt-in through `--allocation_strategy=fragmentation_aware`.
- Preferred and excluded segment semantics are preserved.
- Replica best-effort behavior is preserved.
- Offset allocators provide exact largest-free-region data.
- CacheLib allocators report unknown largest-free-region data, so the strategy falls back to aggregate free bytes for those allocators.
- If no candidate can fit, the strategy cannot manufacture capacity; it still falls back to existing random best-effort behavior.
