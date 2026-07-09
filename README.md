# Mooncake Fragmentation-Aware Allocation Strategy

Competition: The 8th CCF Open Source Innovation Competition, Open Source Task Challenge Track.

Task: Mooncake KVCache storage design and performance optimization.

## Competition-Ready Positioning

This repository is a CCF Mooncake submission package, not a standalone fork of
the full upstream Mooncake repository. The core source-code artifact is:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

Public GitHub repository:

`https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`

For final competition submission, this package should be mirrored to a public
GitHub repository, and the patch should ideally be applied to a Mooncake fork
branch or draft PR. See:

- `DESIGN.md`
- `EVALUATION.md`
- `SUBMISSION.md`
- `GITHUB_RELEASE_GUIDE.md`

## Official Topic-2 Alignment - 2026-07-06

This package is now explicitly aligned to official Mooncake track `track2_2026Mooncake`, **赛题2：优化 Mooncake Store 吞吐性能、高可用功能和可扩展性，优化 SGLang HiCache + Mooncake Store 性能**.

The precise positioning is:

`Mooncake Store fragmentation-aware allocation for Store scalability/performance stability`

This is not a generic KVCache proposal. The implemented patch targets Mooncake Store segment allocation for mixed-size KVCache objects. The expected competition value is fewer avoidable failed allocation attempts, lower fallback pressure, and more stable large-object placement when long-running Store memory pools become fragmented. SGLang HiCache relevance is through its Mooncake Store backend path; no real SGLang HiCache benchmark is claimed.

New topic-alignment artifacts:

- `OFFICIAL_TOPIC_ALIGNMENT.md`
- `topic_alignment_metrics_20260706.md`
- `repro/topic_aligned_store_scalability_sim.cpp`
- `logs/topic_aligned_store_scalability_sim_20260706.log`
- `TOPIC_ALIGNMENT_REPORT.md`

## Work Summary

This submission adds a new Mooncake Store allocation strategy named `fragmentation_aware`. The strategy improves allocation decisions for mixed-size KV cache workloads by considering the largest contiguous free region in each candidate segment, not only the aggregate free-space ratio.

The feature is designed to reduce failed allocation attempts in long-running clusters where memory becomes fragmented. It keeps Mooncake's existing best-effort replica semantics and uses a bounded sampled candidate set to avoid full-cluster scans.

## Current PR Status - 2026-07-09

Mooncake draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

Current PR head:

`0123fa1 Fix fragmentation-aware allocation test setup`

GitHub Actions status:

`All checks have passed: 26 successful checks, 1 skipped check.`

## Nightly Update - 2026-07-03

This package retains the 2026-07-03 PR-ready patch for traceability:

`mooncake_fragmentation_aware_pr_ready_20260703.patch`

Baseline:

`kvcache-ai/Mooncake@a325291c6baccc872ce137bd0c58d5791ac4e8c4`

The earlier `mooncake_fragmentation_aware.patch` is also retained for
traceability. The current preferred review artifact is
`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`, matching Mooncake draft
PR `#2797` after GitHub Actions passed.

Additional evidence added:

- `official_verification_20260703.md`
- `quantitative_metrics_20260703.md`
- `patch_pr_ready_notes_20260703.md`
- `final_report.md`
- `NIGHTLY_IMPROVEMENT_REPORT.md`
- New logs under `logs\*20260703*`
- Final nightly package path and SHA256 are recorded in `NIGHTLY_IMPROVEMENT_REPORT.md`.

## Key Deliverables

- New `AllocationStrategyType::FRAGMENTATION_AWARE`.
- New `FragmentationAwareAllocationStrategy`.
- Master startup support via `--allocation_strategy=fragmentation_aware`.
- Unit test covering a deterministic fragmentation scenario.
- Allocation benchmark matrix support for the new strategy.
- Updated design, deployment, and quick-start documentation.
- TCP-only / old-libibverbs build compatibility fixes needed for local
  validation on Ubuntu 20.04.

## Repository

Current scoped working repository:

`C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake`

Current upstream baseline:

`a325291c6baccc872ce137bd0c58d5791ac4e8c4`

## Usage

Start Mooncake master with:

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

For HTTP metadata quick start:

```bash
mooncake_master \
  --allocation_strategy=fragmentation_aware \
  --enable_http_metadata_server=true \
  --http_metadata_server_port=8080
```

## Status

Implementation patch is prepared at:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

Local verification results are recorded in `C:\CCFOpenSource\02_Mooncake_FragmentationAware\testing.md` and `C:\CCFOpenSource\02_Mooncake_FragmentationAware\logs`.

Primary local validation:

- `allocation_strategy_light_test.log`: passed.
- `fragmentation_aware_sim.log`: passed.
- `fragmentation_aware_sim_verify_20260703_0002.log`: passed.
- `fragmentation_aware_metrics_verify_20260703_0002.log`: passed.
- `topic_aligned_store_scalability_sim_20260706.log`: passed; deterministic topic-2 simulation shows primary fit success improved from 0/6 to 6/6 and fallback attempts reduced from 11 to 0 in the synthetic fragmented Store scenario.
- `git_apply_check_pr_ready_20260703_0002.log`: PR-ready patch apply check passed.
- Full upstream Store target was attempted but did not finish in the local
  WSL/GCC10/Windows-mounted workspace within the available resource window.
