# Final Report: Mooncake Fragmentation-Aware Allocation

## Official Topic-2 Alignment Update - 2026-07-06

This final report is now explicitly aligned to official Mooncake `track2_2026Mooncake` 赛题2:

> 优化 Mooncake Store 吞吐性能、高可用功能和可扩展性，优化 SGLang HiCache + Mooncake Store 性能。

Corrected project positioning:

`Mooncake Store fragmentation-aware allocation for Store scalability/performance stability`

The contribution maps to topic 2 through Mooncake Store's allocation path. In mixed-size KVCache workloads, fragmented memory can make aggregate free-space ranking select a segment that cannot fit the current object, creating avoidable retry/fallback work. The new strategy ranks fit-capable contiguous candidates first while preserving bounded sampling and existing fallback behavior.

New topic-alignment evidence:

- `OFFICIAL_TOPIC_ALIGNMENT.md`
- `topic_alignment_metrics_20260706.md`
- `logs\topic_aligned_store_scalability_sim_20260706.log`

The 2026-07-06 deterministic simulation reports primary fit success improving from 0/6 to 6/6 and fallback attempts reducing from 11 to 0 in a synthetic fragmented Store scenario. This supports Store allocation-path stability and scalability claims only. It does not claim a real SGLang HiCache benchmark or RDMA benchmark.

## PR CI Update - 2026-07-09

Mooncake draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

Current PR head:

`0123fa1 Fix fragmentation-aware allocation test setup`

GitHub Actions result:

`All checks have passed: 26 successful checks, 1 skipped check.`

## Project

CCF 2026 Open Source Task Challenge: Mooncake KVCache storage design and performance optimization.

## Contribution

This work adds an opt-in Mooncake Store allocation strategy:

```bash
--allocation_strategy=fragmentation_aware
```

The strategy targets mixed-size KV cache workloads where aggregate free space is a weak predictor of allocation success. It uses bounded candidate sampling, keeps preferred/excluded segment semantics, and ranks candidates by contiguous fit capability before blending contiguity and free-ratio signals.

## Problem

`free_ratio_first` ranks candidates by aggregate free-space ratio. In a long-running KV cache store, mixed-size allocation and eviction can split free bytes into small holes:

- A fragmented segment can report more total free bytes.
- A less fragmented segment can report fewer total free bytes but have a larger contiguous free region.
- Large KV cache objects must fit in one allocator region.

Therefore, a high free ratio can still produce a failed allocation attempt when the largest contiguous region is smaller than the request.

## Algorithm

For each sampled segment:

```text
free_ratio = total_free / total_capacity
contiguity_ratio = largest_free_region / total_free
can_fit = largest_free_region >= request_size
fa_score = 0.70 * contiguity_ratio + 0.30 * free_ratio
```

Ranking:

1. Fit-capable segments first.
2. Higher `fa_score`.
3. Higher `largest_free_region`.
4. Higher `free_ratio`.
5. Stable candidate-index tie break.

Fallback remains the existing random best-effort path.

## Implementation

Preferred PR patch:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

Baseline:

`kvcache-ai/Mooncake@c9896684fbd7b85ca207c643056a645ab6be3bad`

Touched areas:

- Strategy implementation and factory wiring.
- Master CLI/config parsing.
- Unit tests.
- Benchmark matrix inclusion.
- Mooncake Store design/deployment documentation.

## Validation

Passed:

- `git diff --check`.
- GitHub Actions on PR head `0123fa1`: 26 successful checks, 1 skipped check.
- Deterministic standalone fragmentation simulation.
- Extended metrics simulation with 5 deterministic scenarios.

Primary metric:

```text
request_mib=10
fragmented: free_mib=16, largest_mib=8, free_ratio=0.500, fa_score=0.500, can_fit=no
contiguous: free_mib=12, largest_mib=12, free_ratio=0.375, fa_score=0.812, can_fit=yes
free_ratio_first_choice=fragmented can_fit=no
fragmentation_aware_choice=contiguous can_fit=yes
```

Logs:

- `logs\git_apply_check_pr_ready_20260703_0002.log`
- `logs\fragmentation_aware_sim_verify_20260703_0002.log`
- `logs\fragmentation_aware_metrics_verify_20260703_0002.log`

## Reproducibility

Run:

```bash
cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware
./fragmentation_aware_sim_verify_20260703_0002
./fragmentation_aware_metrics_verify_20260703_0002
```

Apply patch:

```bash
git checkout c9896684fbd7b85ca207c643056a645ab6be3bad
git apply --check mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

Rollback:

```bash
git apply -R mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

## Remaining Risk

Full local Mooncake rebuild is still not claimed in this Windows/WSL workspace.
The upstream draft PR has passed GitHub Actions, including the
`allocation_strategy_test` target. Remaining risk is the absence of real RDMA
and SGLang HiCache end-to-end benchmarking in this initial-round package.

## Submission Artifacts

- `README.md`
- `technical_solution.md`
- `testing.md`
- `usage.md`
- `patch_pr_ready_notes_20260703.md`
- `quantitative_metrics_20260703.md`
- `official_verification_20260703.md`
- `final_report.md`
- `NIGHTLY_IMPROVEMENT_REPORT.md`
- `mooncake_fragmentation_aware_pr_2797_0123fa1.patch`
- `logs\`
- `repro\`
