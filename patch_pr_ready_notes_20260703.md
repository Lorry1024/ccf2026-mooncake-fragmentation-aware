# PR-Ready Patch Notes - 2026-07-03

Preferred patch for review:

`mooncake_fragmentation_aware_pr_ready_20260703.patch`

Baseline:

`kvcache-ai/Mooncake@a325291c6baccc872ce137bd0c58d5791ac4e8c4`

The earlier patch `mooncake_fragmentation_aware.patch` is retained for traceability, but it no longer applies cleanly to the latest upstream `main` because the repository evolved around allocation strategy docs/config and transport build files. Use the 2026-07-03 PR-ready patch for new review.

## Files Touched

| File | Purpose |
|---|---|
| `mooncake-store/include/allocation_strategy.h` | Adds `FragmentationAwareAllocationStrategy`, candidate scoring, and factory wiring. |
| `mooncake-store/include/types.h` | Adds `AllocationStrategyType::FRAGMENTATION_AWARE`. |
| `mooncake-store/include/master_config.h` | Parses `allocation_strategy=fragmentation_aware`. |
| `mooncake-store/src/master.cpp` | Adds the new strategy to the CLI flag help text. |
| `mooncake-store/tests/allocation_strategy_test.cpp` | Adds strategy parameterization and deterministic fragmentation/preferred-segment tests. |
| `mooncake-store/benchmarks/allocation_strategy_bench.cpp` | Adds the new strategy to benchmark matrices. |
| `docs/source/design/mooncake-store.md` | Documents selection guidance and algorithm details. |
| `docs/source/deployment/mooncake-store-deployment-guide.md` | Documents the `--allocation_strategy` option. |

## Apply

From a clean Mooncake checkout at the baseline commit:

```bash
git checkout a325291c6baccc872ce137bd0c58d5791ac4e8c4
git apply --check /path/to/mooncake_fragmentation_aware_pr_ready_20260703.patch
git apply /path/to/mooncake_fragmentation_aware_pr_ready_20260703.patch
```

Local verification in this workspace:

```text
git_apply_check_exit=0
```

Log:

`logs\git_apply_check_pr_ready_20260703_0002.log`

## Rollback

Before commit:

```bash
git apply -R /path/to/mooncake_fragmentation_aware_pr_ready_20260703.patch
```

After commit:

```bash
git revert <commit-sha>
```

## Compatibility

- The default strategy remains `random`; behavior changes only when the operator explicitly passes `--allocation_strategy=fragmentation_aware`.
- Preferred segments are still tried before sampled candidates.
- Excluded and already-used segments are skipped.
- Best-effort replica behavior is preserved: partial success is returned when at least one replica is allocated.
- CacheLib allocators do not expose exact fragmentation metadata; the strategy treats unknown largest free region conservatively as available free bytes, matching the existing best-effort allocation contract.
- Offset allocators expose actual largest free region through `getLargestFreeRegion()`, giving the strategy precise fragmentation information.

## Potential Side Effects

- Slightly more metadata reads than `free_ratio_first`, because each sampled candidate reads largest-free-region information in addition to aggregate free bytes.
- When both candidates can fit, a more contiguous segment may outrank a segment with a higher aggregate free ratio. This is intentional for mixed-size churn workloads but may differ from pure utilization-balancing goals.
- In clusters where all allocators report unknown largest free region, behavior approaches free-ratio/contiguity scoring based on aggregate free bytes rather than exact fragmentation.

## Validation Status

- `git diff --check`: passed.
- New PR-ready patch apply check against clean `a325291c...`: passed.
- Standalone fragmentation simulation: passed.
- Extended metrics simulation with 5 deterministic scenarios: passed.
- Header-level light test could not be rebuilt in the local WSL Ubuntu 20.04 environment because current upstream `mutex.h` uses `std::atomic_flag::test`, which is unavailable in the installed libstdc++ used by `g++-10` and `clang++-18`.
