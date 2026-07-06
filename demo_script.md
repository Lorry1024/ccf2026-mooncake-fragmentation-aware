# 5-Minute Demo Script

## 0:00-0:30 Topic

This work is aligned to official Mooncake `track2_2026Mooncake` 赛题2: optimizing Mooncake Store throughput performance, high availability, scalability, and SGLang HiCache + Mooncake Store performance.

The implemented feature is not a generic KVCache idea. It is `Mooncake Store fragmentation-aware allocation for Store scalability/performance stability`, exposed as a new Mooncake Store allocation strategy: `fragmentation_aware`.

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
cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware
./fragmentation_aware_sim_verify_20260703_0002
./fragmentation_aware_metrics_verify_20260703_0002
./topic_aligned_store_scalability_sim_20260706
```

Show:

- The deterministic simulation confirms `fragmentation_aware` chooses the contiguous-fit candidate.
- The extended metrics simulation validates 5 ranking and boundary scenarios.
- The topic-aligned Store scalability simulation shows primary fit success 0/6 -> 6/6 and fallback attempts 11 -> 0 in a synthetic fragmented Store scenario, with the same average candidate count.
- `fragmented`: 16 MiB total free, 8 MiB largest region, cannot fit 10 MiB.
- `contiguous`: 12 MiB total free, 12 MiB largest region, can fit 10 MiB.
- `free_ratio_first` chooses `fragmented`.
- `fragmentation_aware` chooses `contiguous`.

Also show:

```bash
git apply --check mooncake_fragmentation_aware_pr_ready_20260703.patch
```

Expected evidence:

- `logs/git_apply_check_pr_ready_20260703_0002.log`
- `quantitative_metrics_20260703.md`

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
- Better allocation reliability under mixed-size KV cache churn, mapped to Mooncake Store throughput stability and scalability for official赛题2.
- Code, test, documentation, and reproduction artifacts are prepared for review.
- Honest boundary: this is not yet a real SGLang HiCache benchmark, not an RDMA benchmark, and not an official CI result.

## Patch Readiness Add-on

If asked about PR readiness:

- The old patch is kept but no longer applies to current upstream `main`.
- The new patch was regenerated against `a325291c6baccc872ce137bd0c58d5791ac4e8c4`.
- It passed `git apply --check` in a clean worktree.
- Light-test rebuild is blocked by the local Ubuntu 20.04 libstdc++ lacking `std::atomic_flag::test`; this is an environment/toolchain limitation in current upstream headers, not a failure in the fragmentation-aware simulation.
