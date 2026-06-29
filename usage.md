# Usage Guide

## Enable the Strategy

```bash
mooncake_master --allocation_strategy=fragmentation_aware
```

With HTTP metadata enabled:

```bash
mooncake_master \
  --allocation_strategy=fragmentation_aware \
  --enable_http_metadata_server=true \
  --http_metadata_server_port=8080
```

## When to Use

Use `fragmentation_aware` when:

- workload object sizes vary significantly.
- the cluster runs long enough for free space to become fragmented.
- large KV pages fail even when aggregate free bytes are still available.

Use `free_ratio_first` when the primary goal is utilization balance and fragmentation is not a concern.

Use `random` when the cluster is stable and the highest raw allocation throughput is preferred.

## Local Verification Commands

```bash
/mnt/c/CCFOpenSource/Work/allocation_strategy_light_test
/mnt/c/CCFOpenSource/Work/fragmentation_aware_sim
```

Expected logs:

- `C:\CCFOpenSource\Submission\logs\allocation_strategy_light_test.log`
- `C:\CCFOpenSource\Submission\logs\fragmentation_aware_sim.log`
