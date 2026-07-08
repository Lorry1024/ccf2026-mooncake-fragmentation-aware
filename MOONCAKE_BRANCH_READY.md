# Mooncake Branch Ready

Local prepared upstream clone:

`E:\26ccf\Mooncake-current-check`

Branch:

`ccf-fragmentation-aware-allocation`

Fork branch:

`https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation`

Mooncake draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

Commit:

`f227c22 Add fragmentation-aware Store allocation strategy`

Status:

- The `mooncake_fragmentation_aware_pr_ready_20260703.patch` changes are
  applied as a real Git commit.
- `git diff --check` passed before packaging.
- The branch has been pushed to the `Lorry1024/Mooncake` fork.
- Draft PR created: `https://github.com/kvcache-ai/Mooncake/pull/2797`.

If the branch needs to be pushed again:

```bash
git push lorry ccf-fragmentation-aware-allocation
```

Draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

Primary submission package:

`release/CCF2026_Mooncake_FragmentationAware_initial_20260707.zip`

Suggested PR description:

```text
## Summary

This PR adds an opt-in Mooncake Store allocation strategy named
fragmentation_aware for mixed-size KVCache workloads. The strategy keeps the
bounded candidate sampling shape of free_ratio_first, but ranks sampled
segments by whether their largest contiguous free region can satisfy the
current request before considering aggregate free ratio.

## Why

In long-running Store pools, aggregate free space can be split into fragmented
holes. A segment may report a higher total free ratio while failing a large
allocation because no contiguous region is large enough. This creates avoidable
allocation attempts and fallback pressure.

## Changes

- Add AllocationStrategyType::FRAGMENTATION_AWARE.
- Add FragmentationAwareAllocationStrategy.
- Wire --allocation_strategy=fragmentation_aware.
- Add deterministic fragmentation and preferred-segment tests.
- Add the strategy to allocation_strategy_bench.
- Document design and deployment usage.

## Validation

- git diff --check
- git apply --check against current upstream
- deterministic fragmentation simulation
- extended ranking and boundary simulation
- topic-aligned Store scalability simulation

## Boundaries

The default allocation strategy remains unchanged. This PR does not redesign
SGLang HiCache, RDMA transport, or Mooncake HA. Full upstream CI, RDMA
validation, and real SGLang HiCache benchmark should still be run in a
production-like environment before claiming end-to-end throughput gains.
```
