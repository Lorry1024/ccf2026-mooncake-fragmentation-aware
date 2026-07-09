# Usage Guide

## Official Topic-2 Usage Positioning

Use this package as a Mooncake Store allocation-strategy patch for official赛题2 alignment:

`Mooncake Store fragmentation-aware allocation for Store scalability/performance stability`

The strategy is relevant when SGLang HiCache or another KVCache frontend uses Mooncake Store as the storage backend and the Store sees mixed-size KVCache objects over a long-running process lifetime. It is not a standalone SGLang change and no real SGLang HiCache benchmark is included.

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
cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware
./fragmentation_aware_sim_verify_20260703_0002
./fragmentation_aware_metrics_verify_20260703_0002
```

Expected logs:

- `C:\CCFOpenSource\02_Mooncake_FragmentationAware\logs\fragmentation_aware_sim_verify_20260703_0002.log`
- `C:\CCFOpenSource\02_Mooncake_FragmentationAware\logs\fragmentation_aware_metrics_verify_20260703_0002.log`

## PR-Ready Patch Usage

For current upstream Mooncake `main` at:

```text
a325291c6baccc872ce137bd0c58d5791ac4e8c4
```

use:

```text
mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

Apply:

```bash
git checkout c9896684fbd7b85ca207c643056a645ab6be3bad
git apply --check /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git apply /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

Rollback before committing:

```bash
git apply -R /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
```

Nightly local verification commands from this package:

```bash
cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware
./fragmentation_aware_sim_verify_20260703_0002
./fragmentation_aware_metrics_verify_20260703_0002
./topic_aligned_store_scalability_sim_20260706
```

Expected logs:

- `logs\fragmentation_aware_sim_verify_20260703_0002.log`
- `logs\fragmentation_aware_metrics_verify_20260703_0002.log`
- `logs\topic_aligned_store_scalability_sim_20260706.log`
- `logs\git_apply_check_pr_ready_20260703_0002.log`
