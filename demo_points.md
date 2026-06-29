# Demo and Defense Points

## 5-Minute Demo Structure

1. Competition task and selected Mooncake Store optimization direction.
2. Problem: aggregate free space can hide fragmentation.
3. Implementation: `fragmentation_aware` allocation strategy.
4. Code walkthrough:
   - enum/config/flag support.
   - candidate scoring.
   - deterministic fragmentation unit test.
   - benchmark matrix integration.
5. Test run and result logs.
6. Why this improves Mooncake Store for mixed-size KV cache workloads.

## Key Claims to Support

- The feature is opt-in and preserves existing default behavior.
- Selection overhead is bounded by the same small candidate sample size as `free_ratio_first`.
- The lightweight unit test and standalone simulation demonstrate a concrete failure mode that total-free-space ranking cannot distinguish.
- Documentation makes the feature reproducible for users and reviewers.
