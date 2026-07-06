# Nightly Improvement Report - 2026-07-03

Scope: `C:\CCFOpenSource\02_Mooncake_FragmentationAware`

Hard-boundary note: all local reads/writes for this run were kept inside this scoped directory. Existing packages were preserved.

## Summary

This run strengthened the Mooncake Fragmentation-Aware Allocation submission in four ways:

1. Re-checked official/public sources and current Mooncake upstream.
2. Discovered that the old patch no longer applies cleanly to current upstream `main`.
3. Regenerated a smaller PR-ready patch against current upstream `a325291c6baccc872ce137bd0c58d5791ac4e8c4`.
4. Added deterministic quantitative evidence for the fragmentation failure mode and boundary behavior.

## Files Changed or Added

Added:

- `official_verification_20260703.md`
- `quantitative_metrics_20260703.md`
- `patch_pr_ready_notes_20260703.md`
- `final_report.md`
- `final_check_report_20260703.md`
- `repro\fragmentation_aware_metrics.cpp`
- `mooncake_fragmentation_aware_pr_ready_20260703.patch`
- `Mooncake_FragmentationAware_submission_nightly_20260703_0001.zip`
- `Mooncake_FragmentationAware_submission_nightly_20260703_0002.zip`
- `Mooncake_FragmentationAware_submission_nightly_20260703_0003.zip`

Updated:

- `README.md`
- `technical_solution.md`
- `testing.md`
- `usage.md`
- `demo_script.md`
- `demo_points.md`
- `submission_checklist.md`
- `final_check_report.md`

New logs:

- `logs\git_apply_check_20260703_0001.log`
- `logs\git_apply_check_pr_ready_20260703_0001.log`
- `logs\git_apply_check_pr_ready_20260703_0002.log`
- `logs\git_diff_check_pr_ready_20260703_0002.log`
- `logs\fragmentation_aware_sim_verify_20260703_0001.log`
- `logs\fragmentation_aware_sim_verify_20260703_0002.log`
- `logs\fragmentation_aware_metrics_verify_20260703_0001.log`
- `logs\fragmentation_aware_metrics_verify_20260703_0002.log`
- `logs\allocation_strategy_light_test_verify_20260703_0001.log`
- `logs\allocation_strategy_light_test_verify_20260703_0002.log`
- `logs\allocation_strategy_light_test_verify_20260703_0003.log`

## Commands Run

Official/repository checks:

```powershell
curl.exe -I -L --max-time 20 https://www.gitlink.org.cn/competitions/2026
curl.exe -I -L --max-time 20 https://www.gitlink.org.cn/competitions/track2_2026Mooncake
curl.exe -L --max-time 30 https://www.ccf.org.cn/kyfzwyh/wyhdt/2026-03-23/865095.shtml
git ls-remote https://github.com/kvcache-ai/Mooncake.git HEAD refs/heads/main
```

Patch work:

```powershell
git clone --depth 1 --filter=blob:none https://github.com/kvcache-ai/Mooncake.git C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake
git -C C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake submodule update --init --depth 1 extern/yalantinglibs
git -C C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake diff --check
git -C C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake diff --binary --output=C:\CCFOpenSource\02_Mooncake_FragmentationAware\mooncake_fragmentation_aware_pr_ready_20260703.patch
git -C C:\CCFOpenSource\02_Mooncake_FragmentationAware\upstream\Mooncake_patch_check_20260703_0001 apply --check --verbose C:\CCFOpenSource\02_Mooncake_FragmentationAware\mooncake_fragmentation_aware_pr_ready_20260703.patch
```

Simulation:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=c++17 -O2 repro/fragmentation_aware_sim.cpp -o fragmentation_aware_sim_verify_20260703_0002 && ./fragmentation_aware_sim_verify_20260703_0002"
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=c++17 -O2 repro/fragmentation_aware_metrics.cpp -o fragmentation_aware_metrics_verify_20260703_0002 && ./fragmentation_aware_metrics_verify_20260703_0002"
```

Light test attempts:

```powershell
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++ -std=gnu++20 ..."
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && g++-10 -std=gnu++20 -fcoroutines ..."
wsl bash -lc "cd /mnt/c/CCFOpenSource/02_Mooncake_FragmentationAware && /usr/lib/llvm-18/bin/clang++ -std=gnu++20 ..."
```

Package:

```powershell
Compress-Archive -Path <submission files> -DestinationPath C:\CCFOpenSource\02_Mooncake_FragmentationAware\Mooncake_FragmentationAware_submission_nightly_20260703_0003.zip -CompressionLevel Optimal
Get-FileHash -Algorithm SHA256 C:\CCFOpenSource\02_Mooncake_FragmentationAware\Mooncake_FragmentationAware_submission_nightly_20260703_0003.zip
```

## Test Results

| Check | Result | Log |
|---|---|---|
| Old patch apply check against current upstream | Failed, as expected after upstream drift | `logs\git_apply_check_20260703_0001.log` |
| New PR-ready patch apply check | Passed | `logs\git_apply_check_pr_ready_20260703_0002.log` |
| `git diff --check` | Passed | `logs\git_diff_check_pr_ready_20260703_0002.log` |
| Standalone simulation | Passed, exit 0 | `logs\fragmentation_aware_sim_verify_20260703_0002.log` |
| Extended metrics simulation | Passed, exit 0 | `logs\fragmentation_aware_metrics_verify_20260703_0002.log` |
| Header-level light test rebuild | Blocked by local libstdc++ C++20 support | `logs\allocation_strategy_light_test_verify_20260703_0002.log`, `logs\allocation_strategy_light_test_verify_20260703_0003.log` |

## Quantitative Metrics

Primary deterministic case:

| Segment | Request MiB | Free MiB | Largest MiB | Free Ratio | Contiguity Ratio | FA Score | Can Fit |
|---|---:|---:|---:|---:|---:|---:|---|
| fragmented | 10 | 16 | 8 | 0.500 | 0.500 | 0.500 | no |
| contiguous | 10 | 12 | 12 | 0.375 | 1.000 | 0.812 | yes |

Outcome:

```text
free_ratio_first_choice=fragmented can_fit=no
fragmentation_aware_choice=contiguous can_fit=yes
```

Extended scenarios:

- `mixed_10m`
- `boundary_8m`
- `boundary_12m`
- `no_fit_20m`
- `zero_free_guard`

Full table: `quantitative_metrics_20260703.md`.

## Final Package

Path:

`C:\CCFOpenSource\02_Mooncake_FragmentationAware\Mooncake_FragmentationAware_submission_nightly_20260703_0003.zip`

Size:

`132401` bytes

SHA256:

`BB341DFAC1764FD5C6A884904F03DD669FA0CA6D19343B70A46B9400CD1DA818`

Sidecar:

`C:\CCFOpenSource\02_Mooncake_FragmentationAware\Mooncake_FragmentationAware_submission_nightly_20260703_0003.zip.sha256.txt`

Note: this report was written after the package hash was computed, so the report itself is intentionally outside that zip to avoid a circular hash dependency. The report should be submitted alongside the zip if nightly provenance is required.

## Remaining Risks

- Full upstream `allocation_strategy_test` and benchmark binaries were not rebuilt locally.
- Local WSL Ubuntu 20.04 uses an older libstdc++ where `std::atomic_flag::test` is unavailable, blocking current upstream header-level compilation before the new strategy is exercised.
- GitLink anonymous task-detail API returned a server/device time mismatch response, so hidden scoring details were not asserted. Public CCF notice, GitLink URLs, Mooncake repository, Mooncake docs, and local run logs were used as the evidence base.
- Before opening an upstream PR, rerun full CMake/unit/benchmark validation in a native Linux or Mooncake CI-like environment with a newer standard library.
