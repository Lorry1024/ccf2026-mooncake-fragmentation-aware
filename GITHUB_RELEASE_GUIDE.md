# GITHUB RELEASE GUIDE

## Why This Is Needed

The competition guide requires all results to be hosted publicly on GitHub.
The current GitLink repository and local patch package are useful, but the
final submission should also have a GitHub-visible source of truth.

## Recommended Publication Plan

1. Push the contents of this package to `https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`.
2. Fork `kvcache-ai/Mooncake` on GitHub.
3. Push a branch with the applied `fragmentation_aware` changes.
4. Draft PR has been created: `https://github.com/kvcache-ai/Mooncake/pull/2797`.
5. PR CI has passed on `0123fa1`: 26 successful checks, 1 skipped check.
6. Put both links in the GitLink platform submission.

## Local Mooncake Branch Preparation

Current pushed fork branch:

`https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation`

Current draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

Current PR head:

`0123fa1 Fix fragmentation-aware allocation test setup`

Current patch artifact:

`mooncake_fragmentation_aware_pr_2797_0123fa1.patch`

From a clean Mooncake checkout:

```bash
git checkout -b ccf-fragmentation-aware-allocation
git apply /path/to/mooncake_fragmentation_aware_pr_2797_0123fa1.patch
git diff --check
git add .github/workflows/ci.yml \
        docs/source/deployment/mooncake-store-deployment-guide.md \
        docs/source/design/mooncake-store.md \
        mooncake-store/benchmarks/allocation_strategy_bench.cpp \
        mooncake-store/include/allocation_strategy.h \
        mooncake-store/include/master_config.h \
        mooncake-store/include/types.h \
        mooncake-store/src/master.cpp \
        mooncake-store/tests/allocation_strategy_test.cpp
git commit -m "Add fragmentation-aware Store allocation strategy"
```

Suggested branch name:

`ccf-fragmentation-aware-allocation`

Suggested PR title:

`Add fragmentation-aware allocation strategy for Mooncake Store`

## Suggested PR Description

```text
## Summary

This PR adds an opt-in Mooncake Store allocation strategy named
fragmentation_aware. It is designed for mixed-size KVCache workloads where
aggregate free ratio can be misleading under allocator fragmentation.

The strategy preserves preferred/excluded segment semantics and best-effort
replica behavior, while ranking sampled candidates by contiguous-fit capability
before aggregate free ratio.

## Validation

- git diff --check
- GitHub Actions on PR head 0123fa1: 26 successful checks, 1 skipped check
- deterministic fragmentation simulation
- extended ranking and boundary simulation
- topic-aligned Store scalability simulation

## Boundaries

This PR does not change the default allocation strategy, redesign SGLang
HiCache, or introduce a new HA protocol. Full RDMA and SGLang HiCache benchmark
validation should be run in a production-like environment before claiming
end-to-end throughput gains.
```

## GitHub Submission Package

After pushing the submission repository, create a release or tag:

```bash
git tag ccf2026-mooncake-initial
git push origin ccf2026-mooncake-initial
```

Attach or link the final zip package and SHA256 file in the GitHub release if
desired.
