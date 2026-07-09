# Review Guide

This file is the recommended reading order for CCF initial-round reviewers.

## Quick Links

- Public submission repository:
  `https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`
- Mooncake draft PR:
  `https://github.com/kvcache-ai/Mooncake/pull/2797`
- Mooncake fork branch:
  `https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation`

## Current PR Status

- PR head: `0123fa1 Fix fragmentation-aware allocation test setup`
- GitHub Actions: `26 successful checks, 1 skipped check`
- PR mode: draft, pending upstream maintainer review

## Recommended Reading Order

1. `README.md` for the project summary and current PR status.
2. `DESIGN.md` for the fragmentation-aware allocation design.
3. `EVALUATION.md` for reproducible evidence and remaining boundaries.
4. `mooncake_fragmentation_aware_pr_2797_0123fa1.patch` for the exact current
   PR diff.
5. `slides/Mooncake_FragmentationAware_初赛展示.pptx` for presentation material.
6. `demo_script.md` and `demo_points.md` for the pending 5-minute video.

## Scope Boundary

The submitted work optimizes the Mooncake Store allocation path for fragmented,
mixed-size KVCache object placement. It does not claim a full SGLang HiCache
end-to-end benchmark, RDMA benchmark, or Mooncake HA redesign in the initial
round.
