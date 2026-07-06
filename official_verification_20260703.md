# Official Verification Notes - 2026-07-03

## Competition Sources Checked

- CCF official notice: https://www.ccf.org.cn/kyfzwyh/wyhdt/2026-03-23/865095.shtml
- GitLink competition entry: https://www.gitlink.org.cn/competitions/2026
- GitLink Mooncake task entry: https://www.gitlink.org.cn/competitions/track2_2026Mooncake
- Mooncake upstream repository: https://github.com/kvcache-ai/Mooncake
- Mooncake Store design guide: https://kvcache-ai.github.io/Mooncake/design/mooncake-store.html
- Mooncake Store deployment guide: https://github.com/kvcache-ai/Mooncake/blob/main/docs/source/deployment/mooncake-store-deployment-guide.md

## Verified Facts

- The CCF notice identifies the event as the 8th CCF Open Source Innovation Competition, organized by CCF and CCF Open Source Development Committee.
- The notice lists `Mooncake KVCache存储设计和性能优化赛题` under the Open Source Task Challenge track, with OpenAnolis/Dragon Lizard community as the proposer.
- The notice states the GitLink entry as `https://www.gitlink.org.cn/competitions/2026`.
- The notice states the recruitment/submission window as 2026-03-20 to 2026-07-10, with work submission due on 2026-07-10.
- The Mooncake GitHub repository was reachable and `origin/main` resolved to commit `a325291c6baccc872ce137bd0c58d5791ac4e8c4` during this run.
- Mooncake Store is documented as a distributed KV cache storage engine for LLM inference. The Master Service is responsible for object space allocation and metadata, while client nodes contribute memory segments to the cache pool.
- Mooncake Store documents preferred segment allocation through the allocation strategy system and `ReplicateConfig::preferred_segment`.

## Anonymous Access Limitation

The GitLink SPA pages returned HTTP 200, but anonymous API probes returned a server/device time mismatch response in this environment. Therefore, this report does not assert any task-specific hidden scoring rubric beyond what was available from the official CCF notice, public GitLink URLs, Mooncake repository, Mooncake documentation, and local validation results.

## Consequence for Submission

The strengthened submission focuses on reviewable engineering evidence:

- A PR-ready patch against current Mooncake `main`.
- Deterministic simulation and metrics logs for the target fragmentation failure mode.
- Clear reproduction commands and environment limitations.
- Patch apply, rollback, compatibility, and risk notes.
