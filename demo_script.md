# 5-Minute Demo Script

## 0:00-0:30 Topic

This work targets the Mooncake KVCache storage design and performance optimization task. The implemented feature is a new Mooncake Store allocation strategy: `fragmentation_aware`.

## 0:30-1:20 Problem

The existing `free_ratio_first` strategy ranks segments by aggregate free-space ratio. In mixed-size KV cache workloads, aggregate free space can be fragmented into small holes. A segment may report more total free bytes but still fail a large allocation because its largest contiguous free region is too small.

## 1:20-2:30 Implementation

Show the patch:

- `AllocationStrategyType::FRAGMENTATION_AWARE`.
- `FragmentationAwareAllocationStrategy`.
- `--allocation_strategy=fragmentation_aware`.
- Benchmark matrix integration.
- Unit test `FragmentationAwarePrefersContiguousSpaceOverTotalFreeSpace`.

Explain the ranking:

- Fit-capable segments first.
- Score = `0.70 * contiguity_ratio + 0.30 * free_ratio`.
- Same best-effort fallback behavior as existing strategies.

## 2:30-3:30 Reproduction

Run:

```bash
/mnt/c/CCFOpenSource/Work/allocation_strategy_light_test
/mnt/c/CCFOpenSource/Work/fragmentation_aware_sim
```

Show:

- The light unit test passes and confirms `fragmentation_aware` chooses the contiguous-fit candidate while preserving preferred segments.
- `fragmented`: 16 MiB total free, 8 MiB largest region, cannot fit 10 MiB.
- `contiguous`: 12 MiB total free, 12 MiB largest region, can fit 10 MiB.
- `free_ratio_first` chooses `fragmented`.
- `fragmentation_aware` chooses `contiguous`.

## 3:30-4:20 Documentation

Show updated docs:

- Design guide.
- Deployment guide.
- Quick Start.
- Submission testing notes and logs.

## 4:20-5:00 Value

Summarize:

- Opt-in feature, default behavior unchanged.
- Bounded sampling overhead.
- Better allocation reliability under mixed-size KV cache churn.
- Code, test, documentation, and reproduction artifacts are prepared for review.
