# DESIGN: Mooncake Store Fragmentation-Aware Allocation

## Selected Track

This work targets Mooncake CCF topic 2:

`Mooncake Store performance, high availability, scalability, and SGLang HiCache + Mooncake Store optimization`.

The concrete contribution is:

`Mooncake Store fragmentation-aware allocation for mixed-size KVCache objects`.

It maps most directly to the official Store evolution task:

`OffsetAllocator optimization: reduce memory fragmentation for KVCache objects with multiple size classes`.

## Problem

Mooncake Store already supports allocation strategies such as `random` and
`free_ratio_first`. `free_ratio_first` ranks candidate segments by aggregate
free-space ratio, which is useful for utilization balancing.

For long-running KVCache workloads, object sizes vary and allocation/free churn
can split free bytes into small holes. A segment can therefore have a high total
free ratio while still failing a large allocation because its largest contiguous
free region is too small.

That failure mode creates avoidable allocation attempts and fallback pressure on
the Store allocation path.

## Design Goals

- Preserve the existing default behavior unless users opt in.
- Keep the same bounded candidate-sampling shape as `free_ratio_first`.
- Prefer segments that can fit the current object in a contiguous free region.
- Preserve preferred segment, excluded segment, and best-effort replica behavior.
- Avoid introducing new heavyweight dependencies or cross-module rewrites.

## Proposed Strategy

The new strategy is exposed as:

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

For each sampled candidate segment, the strategy computes:

```text
total_free = sum(capacity - used)
largest_free_region = max(allocator.getLargestFreeRegion())
free_ratio = total_free / total_capacity
contiguity_ratio = largest_free_region / total_free
can_fit = largest_free_region >= request_size
score = 0.70 * contiguity_ratio + 0.30 * free_ratio
```

Candidates are ranked by:

1. `can_fit` first.
2. Higher fragmentation-aware score.
3. Larger contiguous free region.
4. Higher aggregate free ratio.
5. Stable candidate index tie-break.

If the ranked candidates cannot satisfy all replicas, the strategy falls back to
the existing random best-effort path.

## Code Changes

Preferred review artifact:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

Touched upstream Mooncake areas:

| Upstream file | Purpose |
| --- | --- |
| `mooncake-store/include/allocation_strategy.h` | Adds `FragmentationAwareAllocationStrategy` and candidate scoring. |
| `mooncake-store/include/types.h` | Adds `AllocationStrategyType::FRAGMENTATION_AWARE`. |
| `mooncake-store/include/master_config.h` | Parses `allocation_strategy=fragmentation_aware`. |
| `mooncake-store/src/master.cpp` | Documents the new CLI flag value. |
| `mooncake-store/tests/allocation_strategy_test.cpp` | Adds deterministic fragmentation and preferred-segment tests. |
| `mooncake-store/benchmarks/allocation_strategy_bench.cpp` | Adds the new strategy to benchmark matrices. |
| `docs/source/design/mooncake-store.md` | Documents the selection guidance and algorithm. |
| `docs/source/deployment/mooncake-store-deployment-guide.md` | Documents deployment usage. |

## Compatibility

- The default strategy remains `random`.
- The new strategy is opt-in through `--allocation_strategy=fragmentation_aware`.
- Existing preferred and excluded segment semantics are preserved.
- Existing best-effort replica semantics are preserved.
- Allocators without precise largest-free-region metadata degrade to an
  aggregate-free-space interpretation.

## Known Boundaries

- This patch does not redesign SGLang HiCache, RDMA transport, or Mooncake HA.
- The current local evidence is strongest for allocation-path behavior, not
  end-to-end production QPS.
- Upstream GitHub Actions passed on PR head `0123fa1` with 26 successful checks
  and 1 skipped check.
- RDMA benchmark and real SGLang HiCache benchmark are not claimed in this
  initial-round package.
