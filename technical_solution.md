# Technical Solution

## Background

Mooncake Store already provides `random` and `free_ratio_first` allocation strategies. `free_ratio_first` improves cluster utilization balance by sampling candidate segments and ranking them by free-space ratio.

For mixed-size KV cache workloads, aggregate free ratio can be misleading. A segment may have high total free space split across small holes, while another segment with slightly less total free space may have a contiguous region large enough for the current request.

## Design

`fragmentation_aware` extends the existing sampled-selection design:

1. Try preferred segments first, preserving existing behavior.
2. Sample `min(6 * remaining_replicas, total_segments)` candidate segments.
3. For each candidate, compute:
   - total capacity.
   - total free bytes.
   - largest free region across its allocators.
   - whether the largest region can fit the requested slice.
4. Sort candidates by:
   - fit-capable segments first.
   - blended score: `0.70 * contiguity_ratio + 0.30 * free_ratio`.
   - largest free region as a tie-breaker.
5. Attempt allocation in ranked order.
6. Fall back to random allocation for remaining replicas.

## Expected Benefit

The strategy avoids selecting segments that look healthy by total free space but cannot satisfy large allocations due to fragmentation. This should reduce failed allocation attempts under mixed-size object churn and improve usable capacity for KV cache workloads.

## Scope Control

The implementation intentionally avoids changing allocator internals, protocol behavior, or client APIs. It adds one strategy behind an explicit master flag, so existing deployments keep the `random` default unless they opt in.

## Local Build Compatibility

During validation on Ubuntu 20.04, several unrelated build-environment issues blocked a direct Store test binary. The patch includes scoped compatibility fixes:

- `USE_RDMA=OFF` now excludes RDMA transport objects and RPC RDMA initialization paths for TCP-only builds.
- Older `libibverbs` is detected and yalantinglibs is configured to avoid `ibv_query_gid_ex` paths.
- `MasterAdminServer` uses `std::binary_semaphore` when available and a condition-variable fallback on older libstdc++.
- Yalantinglibs source include roots are added explicitly for builds that use the vendored headers.

These changes are build-scope support for reproducibility; they do not change the allocation strategy semantics.
