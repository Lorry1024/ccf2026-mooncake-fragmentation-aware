# Mooncake Fragmentation-Aware Allocation Strategy

Competition: The 8th CCF Open Source Innovation Competition, Open Source Task Challenge Track.

Task: Mooncake KVCache storage design and performance optimization.

## Work Summary

This submission adds a new Mooncake Store allocation strategy named `fragmentation_aware`. The strategy improves allocation decisions for mixed-size KV cache workloads by considering the largest contiguous free region in each candidate segment, not only the aggregate free-space ratio.

The feature is designed to reduce failed allocation attempts in long-running clusters where memory becomes fragmented. It keeps Mooncake's existing best-effort replica semantics and uses a bounded sampled candidate set to avoid full-cluster scans.

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

Main working repository:

`C:\CCFOpenSource\Repos\Mooncake-github`

Working branch:

`ccf-fragmentation-aware-allocation`

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

`C:\CCFOpenSource\Submission\mooncake_fragmentation_aware.patch`

Local verification results are recorded in `C:\CCFOpenSource\Submission\testing.md` and `C:\CCFOpenSource\Submission\logs`.

Primary local validation:

- `allocation_strategy_light_test.log`: passed.
- `fragmentation_aware_sim.log`: passed.
- Full upstream Store target was attempted but did not finish in the local
  WSL/GCC10/Windows-mounted workspace within the available resource window.
