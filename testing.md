# Testing Notes

## Official Topic-2 Alignment Validation - 2026-07-06

This section was added to make the validation evidence explicitly align with official Mooncake `track2_2026Mooncake` 赛题2: Mooncake Store throughput performance, high availability, scalability, and SGLang HiCache + Mooncake Store performance.

New deterministic simulation:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=c++17 -O2 repro/topic_aligned_store_scalability_sim.cpp -o topic_aligned_store_scalability_sim_20260706 && ./topic_aligned_store_scalability_sim_20260706 > logs/topic_aligned_store_scalability_sim_20260706.log 2>&1"
```

Result:

- Exit code: 0.
- Log: `logs\topic_aligned_store_scalability_sim_20260706.log`.
- Summary: `PASS topic_aligned_store_scalability_sim`.

Key metrics:

| Metric | free_ratio_first | fragmentation_aware |
|---|---:|---:|
| Large-object primary fit success | 0/6 | 6/6 |
| Eventual fit success with fallback scan | 6/6 | 6/6 |
| Fallback attempts | 11 | 0 |
| Average candidates scored | 5.00 | 5.00 |
| Decision time per request | 121.00 ns | 138.93 ns |
| Extra local decision cost | n/a | 17.93 ns |

Interpretation: in the deterministic fragmented Store model, `fragmentation_aware` selects a contiguous-fit segment first for all tested large-object requests, while `free_ratio_first` repeatedly picks fragmented high-free segments first and needs fallback attempts. This supports the topic-2 narrative of Store throughput stability and scalability, but it is not a real Mooncake/SGLang HiCache benchmark.

Boundary: upstream GitHub Actions passed on PR head `0123fa1` with 26
successful checks and 1 skipped check. RDMA validation and real SGLang HiCache
benchmark are still not claimed in this local environment.

## Unit Test Added

`FragmentationAwarePrefersContiguousSpaceOverTotalFreeSpace`

Scenario:

- Segment `fragmented`: 40 MiB capacity, five 8 MiB blocks allocated, then two
  interior blocks freed. It has 16 MiB total free space but only 8 MiB largest
  contiguous free region.
- Segment `contiguous`: 40 MiB capacity with 28 MiB allocated. It has about
  12 MiB contiguous free space.
- Request: 10 MiB.
- Expected result: `fragmentation_aware` chooses `contiguous`.

## Results

### CMake Configuration

Configured Mooncake with:

- WSL Ubuntu 20.04.
- CMake 3.29.6 from pip.
- GCC/G++ 10.
- Store, Transfer Engine, unit tests, and benchmarks enabled.
- `USE_RDMA=OFF` for TCP-only local validation.
- Rust, EP, P2P Store, etcd, and Redis disabled.
- AWS SDK not found, so S3 snapshot support was disabled by CMake.
- io_uring not found, so io_uring support was disabled by CMake.

Configuration succeeded in the original June 25 workspace. The retained evidence for this scoped package is in:

- `logs\configure_rdmoff.log`
- `logs\configure_rdmoff_debug.log`

### Full Target Build

Command attempted:

```bash
cmake --build build-ccf-final3 --target allocation_strategy_test allocation_strategy_bench -j2
```

Status: not completed in this local environment before the test binary was produced.

Observed environment issues:

- Ubuntu 20.04 `libibverbs-dev` lacks newer GID APIs/types expected by current RDMA paths. The patch adds TCP-only `USE_RDMA=OFF` support and old-GID fallback handling.
- GCC 10/libstdc++ lacks `<semaphore>`. The patch adds a small `BinarySemaphore` compatibility wrapper for `MasterAdminServer`.
- `RelWithDebInfo -j2` eventually hit `cc1plus` killed by memory pressure while compiling the full Store target.
- `Debug + ENABLE_DEBUG_SYMBOLS=OFF + -j1` progressed without new compile errors in the latest log but exceeded a 20-minute local timeout on the Windows-mounted workspace.

Build logs:

- `logs\configure_rdmoff.log`
- `logs\configure_rdmoff_debug.log`
- `logs\build_allocation_strategy_targets_rdmoff.log`
- `logs\build_allocation_strategy_test_rdmoff_debug.log`

### Lightweight Header-Level Unit Test

Compiled directly against Mooncake headers with a fake allocator backend:

```bash
/usr/bin/g++-10 -std=gnu++20 -O0 -g0 -fcoroutines \
  -DYLT_IBVERBS_DONT_SUPPORT_FIND_GID_INDEX \
  -I/mnt/c/CCFOpenSource/Work/local/include \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/extern/yalantinglibs/include \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/extern/yalantinglibs/include/ylt/standalone \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/extern/yalantinglibs/include/ylt/thirdparty \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/mooncake-store/include \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/mooncake-store/include/cachelib_memory_allocator/include \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/mooncake-store/include/cachelib_memory_allocator/fake_include \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/mooncake-store/include/cachelib_memory_allocator \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/mooncake-common/include \
  -I/mnt/c/CCFOpenSource/Repos/Mooncake-github/mooncake-transfer-engine/include \
  -I/usr/include/jsoncpp \
  /mnt/c/CCFOpenSource/Work/allocation_strategy_light_test.cpp \
  -lglog -lgflags -lpthread \
  -o /mnt/c/CCFOpenSource/Work/allocation_strategy_light_test

/mnt/c/CCFOpenSource/Work/allocation_strategy_light_test
```

Result log:

`logs\allocation_strategy_light_test.log`

Observed result:

```text
PASS allocation_strategy_light_test: fragmentation-aware chooses contiguous-fit candidates and preserves preferred segments
```

### Standalone Fragmentation Reproduction

Compiled and ran:

```bash
g++ -std=c++17 -O2 /mnt/c/CCFOpenSource/Work/fragmentation_aware_sim.cpp \
  -o /mnt/c/CCFOpenSource/Work/fragmentation_aware_sim
/mnt/c/CCFOpenSource/Work/fragmentation_aware_sim
```

Result log:

`logs\fragmentation_aware_sim.log`

Observed result:

```text
request_mib=10

segment       free_mib largest_mib  free_ratio    fa_score   can_fit
fragmented          16           8       0.500       0.500        no
contiguous          12          12       0.375       0.812       yes

free_ratio_first_choice=fragmented can_fit=no
fragmentation_aware_choice=contiguous can_fit=yes
```

Interpretation: `free_ratio_first` picks the segment with the higher aggregate free ratio but cannot fit the request. `fragmentation_aware` picks the segment with a lower aggregate free ratio but sufficient contiguous free space, matching the intended behavior.

## Nightly Validation: 2026-07-03

### Upstream Patch Applicability

Upstream checked:

```text
kvcache-ai/Mooncake@a325291c6baccc872ce137bd0c58d5791ac4e8c4
```

Old patch applicability:

- Patch: `mooncake_fragmentation_aware.patch`
- Command: `git apply --check --verbose`
- Result: failed against current upstream `main`.
- Log: `logs\git_apply_check_20260703_0001.log`
- Cause: upstream changed nearby allocation strategy docs/config, `local_first` strategy wiring, and transport build files after the original patch was prepared.

Current PR-ready patch:

- Patch: `mooncake_fragmentation_aware_pr_2797_0123fa1.patch`
- PR: `https://github.com/kvcache-ai/Mooncake/pull/2797`
- Result: GitHub Actions passed on PR head `0123fa1` with 26 successful checks
  and 1 skipped check.

Format check:

```powershell
git -C C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake diff --check
```

Result: passed.

### Standalone Fragmentation Simulation Re-run

Command:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=c++17 -O2 repro/fragmentation_aware_sim.cpp -o fragmentation_aware_sim_verify_20260703_0002 && ./fragmentation_aware_sim_verify_20260703_0002"
```

Result:

- Exit code: 0.
- Log: `logs\fragmentation_aware_sim_verify_20260703_0002.log`.
- Key output: `free_ratio_first_choice=fragmented can_fit=no`; `fragmentation_aware_choice=contiguous can_fit=yes`.

### Extended Quantitative Metrics Simulation

Command:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=c++17 -O2 repro/fragmentation_aware_metrics.cpp -o fragmentation_aware_metrics_verify_20260703_0002 && ./fragmentation_aware_metrics_verify_20260703_0002"
```

Result:

- Exit code: 0.
- Log: `logs\fragmentation_aware_metrics_verify_20260703_0002.log`.
- Summary: `PASS fragmentation_aware_metrics: validated 5 deterministic ranking and boundary scenarios`.
- Metrics table: `quantitative_metrics_20260703.md`.

### Header-Level Light Test Rebuild Attempt

Commands attempted:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=gnu++20 ... repro/allocation_strategy_light_test.cpp ..."
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++-10 -std=gnu++20 -fcoroutines ... repro/allocation_strategy_light_test.cpp ..."
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && /usr/lib/llvm-18/bin/clang++ -std=gnu++20 ... repro/allocation_strategy_light_test.cpp ..."
```

Results:

- Default `g++` is 9.4.0 and does not support the required C++20 flags.
- `g++-10` and `clang++-18` both reached current upstream Mooncake headers but failed on `std::atomic_flag::test` in `mooncake-store/include/mutex.h`.
- The failure is due to the installed Ubuntu 20.04 libstdc++ not providing this C++20 library API. It occurs before validating the new strategy logic.
- Logs:
  - `logs\allocation_strategy_light_test_verify_20260703_0001.log`
  - `logs\allocation_strategy_light_test_verify_20260703_0002.log`
  - `logs\allocation_strategy_light_test_verify_20260703_0003.log`

The existing historical pass log remains available at `logs\allocation_strategy_light_test.log`. Current-night validation relies on the clean patch apply check, standalone deterministic simulation, and extended quantitative metrics simulation.
