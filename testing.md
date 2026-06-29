# Testing Notes

## Unit Test Added

`FragmentationAwarePrefersContiguousSpaceOverTotalFreeSpace`

Scenario:

- Segment `fragmented`: 32 MiB capacity, four 8 MiB blocks allocated, then two blocks freed. It has 16 MiB total free space but only 8 MiB largest contiguous free region.
- Segment `contiguous`: 32 MiB capacity with 20 MiB allocated. It has about 12 MiB contiguous free space.
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

Configuration succeeded in:

`C:\CCFOpenSource\Repos\Mooncake-github\build-ccf-rdmaoff`

Low-memory configuration also succeeded in:

`C:\CCFOpenSource\Repos\Mooncake-github\build-ccf-rdmaoff-debug`

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

- `C:\CCFOpenSource\Submission\logs\configure_rdmoff.log`
- `C:\CCFOpenSource\Submission\logs\configure_rdmoff_debug.log`
- `C:\CCFOpenSource\Submission\logs\build_allocation_strategy_targets_rdmoff.log`
- `C:\CCFOpenSource\Submission\logs\build_allocation_strategy_test_rdmoff_debug.log`

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

`C:\CCFOpenSource\Submission\logs\allocation_strategy_light_test.log`

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

`C:\CCFOpenSource\Submission\logs\fragmentation_aware_sim.log`

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
