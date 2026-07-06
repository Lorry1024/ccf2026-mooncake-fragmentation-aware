# Official Topic Alignment - track2_2026Mooncake

Date: 2026-07-06

## Official Topic Direction

This submission is aligned to the official Mooncake track `track2_2026Mooncake`, specifically:

> 对齐官方赛题2：优化 Mooncake Store 吞吐性能、高可用功能和可扩展性，优化 SGLang HiCache + Mooncake Store 性能。

The local project already records the official GitLink entry and public Mooncake source references in `official_verification_20260703.md`, including:

- GitLink task entry: `https://www.gitlink.org.cn/competitions/track2_2026Mooncake`
- Mooncake upstream repository: `https://github.com/kvcache-ai/Mooncake`
- Mooncake Store design and deployment documentation.

## Scoring Focus

The locally captured competition guide records these scoring dimensions:

| Scoring item | Weight | Relevance to this submission |
|---|---:|---|
| Technical completeness | 30% | PR-ready patch, config/CLI wiring, tests, benchmark matrix entry, docs, reproducible logs. |
| Open-source standardization | 10% | Patch applies to current upstream baseline, rollback path documented, existing style preserved. |
| Scenario fit | 25% | Directly targets Mooncake Store allocation behavior under mixed-size KVCache workloads and long-running fragmented memory pools. |
| Innovation | 35% | Adds fit-aware and fragmentation-aware candidate ranking instead of only aggregate free-ratio ranking. |

## Current Work Alignment

This project should be described as:

> Mooncake Store fragmentation-aware allocation for Store scalability and performance stability.

It should not be presented as a generic KVCache optimization. The concrete target is Mooncake Store's segment selection path for mixed-size KVCache objects, which can affect allocation success, fallback pressure, and throughput stability when the Store runs for a long time and memory becomes fragmented.

| Official topic keyword | Current artifact alignment | Evidence |
|---|---|---|
| Mooncake Store throughput performance | Reduces avoidable failed allocation attempts before fallback, which can reduce allocation-path work under fragmented pools. | `logs/topic_aligned_store_scalability_sim_20260706.log`: free-ratio primary fit 0/6 vs fragmentation-aware 6/6; fallback attempts 11 vs 0 in deterministic simulation. |
| High availability | Indirect alignment only. The strategy preserves best-effort replica behavior and preferred/excluded segment semantics, but it does not implement new HA protocols. | `technical_solution.md`, patch notes, and preserved fallback behavior. |
| Scalability | Uses bounded sampled candidates rather than full-cluster scans; candidate count remains fixed in the simulation. | `avg_candidates_scored_free_ratio=5.00`, `avg_candidates_scored_fragmentation_aware=5.00`. |
| SGLang HiCache + Mooncake Store performance | Partial alignment through the Store backend allocation path used by KVCache objects. No real SGLang HiCache benchmark is claimed. | `testing.md` and this file explicitly state the boundary. |
| Long-running production stability | Targets mixed-size allocation churn where aggregate free space can hide unusable fragmented holes. | `quantitative_metrics_20260703.md` and `topic_alignment_metrics_20260706.md`. |

## What Was Previously Under-Aligned

Before this pass, the submission was technically correct but too broad in wording:

- It described a general "Mooncake KVCache storage design and performance optimization" task.
- It did not consistently name official topic 2.
- It did not explicitly map fragmentation-aware allocation to Store throughput stability, scalability, and SGLang HiCache backend behavior.
- It had metrics for segment choice and fit, but not a dedicated topic-alignment table covering fallback pressure and candidate sampling overhead.

## Evidence Boundary

The following are true and should be stated clearly:

- The PR-ready patch applies to Mooncake upstream baseline `a325291c6baccc872ce137bd0c58d5791ac4e8c4`.
- Local deterministic simulations pass and demonstrate the target failure mode.
- The new 2026-07-06 topic-aligned simulation is local and deterministic.
- No complete upstream Mooncake build is proven in this environment.
- No RDMA benchmark is proven in this environment.
- No real SGLang HiCache + Mooncake Store benchmark is proven in this environment.
- Therefore, performance language must be framed as allocation-path evidence and expected Store stability value, not as production throughput numbers.
