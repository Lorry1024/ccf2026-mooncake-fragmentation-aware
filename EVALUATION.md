# EVALUATION: Reproducibility and Current Evidence

## Scope

This report evaluates the `fragmentation_aware` Mooncake Store allocation
strategy against the existing `free_ratio_first` baseline for fragmented,
mixed-size KVCache placement scenarios.

The current evidence is sufficient to explain and reproduce the target
allocation failure mode. It is not yet a complete replacement for a full
Mooncake Store benchmark or SGLang HiCache benchmark.

## Environment Recorded

Local validation was performed in a Windows workspace with WSL-based Linux
commands for standalone simulations. Historical Mooncake build attempts were
performed with Ubuntu 20.04, GCC/G++ 10, and `USE_RDMA=OFF`.

Important environment boundary:

- Full upstream Store target build did not complete in the local WSL/GCC10
  environment.
- Current upstream headers require C++20 library APIs that were not available
  in the installed libstdc++ during the historical light-test rebuild attempt.

## Patch Applicability

Preferred patch:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

Recorded baseline:

`kvcache-ai/Mooncake@a325291c6baccc872ce137bd0c58d5791ac4e8c4`

Additional current check:

The patch was also checked against current upstream `main` at:

`fbf32ca60f6bb31c96b055ad85604feb95bcbc00`

Result:

`git apply --check` passed.

## Upstream PR CI

Mooncake draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

Current PR head:

`0123fa1 Fix fragmentation-aware allocation test setup`

GitHub Actions result:

`All checks have passed: 26 successful checks, 1 skipped check.`

This verifies the upstream build/test workflow for the submitted PR branch. It
does not replace hardware-specific RDMA validation or a real SGLang HiCache
benchmark.

## Deterministic Fragmentation Simulation

Source:

`repro/fragmentation_aware_sim.cpp`

Purpose:

Show the smallest concrete failure mode where aggregate free ratio is
misleading.

Key case:

| Segment | Total free | Largest free region | Request | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: | ---: | --- | --- |
| `fragmented` | 16 MiB | 8 MiB | 10 MiB | chooses, cannot fit | rejects as first choice |
| `contiguous` | 12 MiB | 12 MiB | 10 MiB | lower free ratio | chooses, can fit |

Expected result:

```text
free_ratio_first_choice=fragmented can_fit=no
fragmentation_aware_choice=contiguous can_fit=yes
```

## Extended Metrics Simulation

Source:

`repro/fragmentation_aware_metrics.cpp`

Log:

`logs/fragmentation_aware_metrics_verify_20260703_0002.log`

Covered scenarios:

- `mixed_10m`
- `boundary_8m`
- `boundary_12m`
- `no_fit_20m`
- `zero_free_guard`

Result:

`PASS fragmentation_aware_metrics: validated 5 deterministic ranking and boundary scenarios`

## Topic-Aligned Store Scalability Simulation

Source:

`repro/topic_aligned_store_scalability_sim.cpp`

Log:

`logs/topic_aligned_store_scalability_sim_20260706.log`

Summary:

| Metric | `free_ratio_first` | `fragmentation_aware` |
| --- | ---: | ---: |
| Large-object primary fit success | 0/6 | 6/6 |
| Eventual fit success with fallback scan | 6/6 | 6/6 |
| Fallback attempts | 11 | 0 |
| Average candidates scored | 5.00 | 5.00 |
| Decision time per request | 121.00 ns | 138.93 ns |
| Extra local decision cost | n/a | 17.93 ns |

Interpretation:

The new strategy finds a contiguous-fit candidate first in the synthetic
fragmented Store model while preserving bounded candidate scoring.

## Current Gaps Against Official Topic-2 Requirements

| Requirement | Current status | Gap |
| --- | --- | --- |
| Functional test | Passed in upstream PR CI | Deterministic simulations pass and GitHub Actions passed on PR head `0123fa1`. |
| Stress test | Missing | No concurrent Store pressure test is currently included. |
| Abnormal scenario validation | Partial | Fallback/no-fit cases are simulated; real failure injection is missing. |
| Benchmark | Partial | Allocation-path simulation exists; put/get throughput and P50/P99 are missing. |
| Memory utilization metric | Partial | Fragmentation metrics exist; real Store memory utilization benchmark is missing. |
| SGLang HiCache validation | Missing | Relevance is via Store backend only; no real HiCache run is claimed. |

## Recommended Next Evaluation Work

1. Run `allocation_strategy_bench` for `random`, `free_ratio_first`, and
   `fragmentation_aware`.
2. Add a lightweight Store put/get benchmark report with throughput, P50/P99,
   allocation failure count, fallback attempts, and memory utilization.
3. Add at least one abnormal scenario: exhausted candidates, excluded segments,
   or simulated segment churn under mixed-size allocation/free operations.
4. If hardware and time permit, validate with SGLang HiCache using Mooncake
   Store as the backend.
