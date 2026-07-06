# Quantitative Fragmentation Metrics - 2026-07-03

## Scoring Formula

For each sampled segment:

- `total_free = sum(capacity - used)` across allocators in the segment.
- `largest_free_region = max(allocator.getLargestFreeRegion())`.
- `free_ratio = total_free / total_capacity`.
- `contiguity_ratio = largest_free_region / total_free`.
- `can_fit = largest_free_region >= request_size`.
- `fa_score = 0.70 * contiguity_ratio + 0.30 * free_ratio`.

Candidate ordering:

1. `can_fit == true` before `can_fit == false`.
2. Higher `fa_score`.
3. Higher `largest_free_region`.
4. Higher `free_ratio`.
5. Stable candidate index tie-break.

## Reproduced Metrics

Source: `logs\fragmentation_aware_metrics_verify_20260703_0002.log`

| Scenario | Request MiB | Segment | Capacity MiB | Free MiB | Largest MiB | Free Ratio | Contiguity Ratio | FA Score | Can Fit | Free-Ratio Choice | Free-Ratio Can Fit | FA Choice | FA Can Fit |
|---|---:|---|---:|---:|---:|---:|---:|---:|---|---|---|---|---|
| mixed_10m | 10 | fragmented | 32 | 16 | 8 | 0.500 | 0.500 | 0.500 | no | fragmented | no | contiguous | yes |
| mixed_10m | 10 | contiguous | 32 | 12 | 12 | 0.375 | 1.000 | 0.812 | yes | fragmented | no | contiguous | yes |
| boundary_8m | 8 | fragmented | 32 | 16 | 8 | 0.500 | 0.500 | 0.500 | yes | fragmented | yes | contiguous | yes |
| boundary_8m | 8 | contiguous | 32 | 12 | 12 | 0.375 | 1.000 | 0.812 | yes | fragmented | yes | contiguous | yes |
| boundary_12m | 12 | fragmented | 32 | 16 | 8 | 0.500 | 0.500 | 0.500 | no | fragmented | no | contiguous | yes |
| boundary_12m | 12 | contiguous | 32 | 12 | 12 | 0.375 | 1.000 | 0.812 | yes | fragmented | no | contiguous | yes |
| no_fit_20m | 20 | fragmented | 32 | 16 | 8 | 0.500 | 0.500 | 0.500 | no | fragmented | no | contiguous | no |
| no_fit_20m | 20 | contiguous | 32 | 12 | 12 | 0.375 | 1.000 | 0.812 | no | fragmented | no | contiguous | no |
| zero_free_guard | 1 | empty | 32 | 0 | 0 | 0.000 | 0.000 | 0.000 | no | single_hole | yes | single_hole | yes |
| zero_free_guard | 1 | single_hole | 32 | 2 | 2 | 0.062 | 1.000 | 0.719 | yes | single_hole | yes | single_hole | yes |

## Interpretation

- The primary failure case is `mixed_10m`: `free_ratio_first` picks `fragmented` because `0.500 > 0.375`, but its largest free region is only 8 MiB and cannot fit a 10 MiB request.
- `fragmentation_aware` picks `contiguous` because it can fit the request and has a higher contiguity-driven score.
- `boundary_12m` shows the same behavior at the exact largest contiguous block boundary of the winning segment.
- `no_fit_20m` confirms the strategy does not invent capacity; it chooses the least fragmented candidate but still reports `can_fit=no`, so normal allocation failure/fallback behavior remains necessary.
- `zero_free_guard` covers zero-free/empty-segment safety and avoids division-by-zero behavior.
