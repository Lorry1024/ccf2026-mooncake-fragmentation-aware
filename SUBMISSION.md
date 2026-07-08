# SUBMISSION: CCF Mooncake Initial-Round Package

## Competition Requirement Mapping

Selected topic:

`赛题2：优化 Mooncake Store 吞吐性能、高可用功能和可扩展性，优化 SGLang HiCache + Mooncake Store 性能`

Submission modes accepted by the competition:

- File plus link.
- Compressed package only.
- Pull request to the open-source project.

Required artifacts:

- Source code.
- Solution document or PPT.
- Demo video within 5 minutes.
- Public GitHub hosting for all artifacts.

## Recommended Submission Mode

Use `file plus link`.

Submit:

- A public GitHub repository link for the submission package.
- A Mooncake fork branch or PR link containing the applied code changes.
- A compressed package containing the docs, patch, reproducible simulations,
  logs, PPT, and video link note.

Keep the patch as a fallback review artifact, not as the only source-code
artifact.

## Current Local Artifacts

Public GitHub submission repository:

`https://github.com/Lorry1024/ccf2026-mooncake-fragmentation-aware`

Mooncake fork branch:

`https://github.com/Lorry1024/Mooncake/tree/ccf-fragmentation-aware-allocation`

Mooncake draft PR:

`https://github.com/kvcache-ai/Mooncake/pull/2797`

| Artifact | Path | Status |
| --- | --- | --- |
| Project README | `README.md` | Present |
| Official design document | `DESIGN.md` | Present |
| Official evaluation report | `EVALUATION.md` | Present |
| Technical solution | `technical_solution.md` | Present |
| Testing notes | `testing.md` | Present |
| Usage guide | `usage.md` | Present |
| Final report | `final_report.md` | Present |
| PR-ready patch | `mooncake_fragmentation_aware_pr_ready_20260703.patch` | Present |
| Reproducible simulations | `repro/` | Present |
| Validation logs | `logs/` | Present |
| Demo script | `demo_script.md` | Present |
| Demo talking points | `demo_points.md` | Present |

## Still Required Before Platform Submission

| Item | Why it matters | Status |
| --- | --- | --- |
| Public GitHub repository | Competition rule says all artifacts must be hosted on GitHub. | Created and pushed. |
| Mooncake fork branch or PR | Stronger open-source evidence than a standalone patch. | Draft PR created: `kvcache-ai/Mooncake#2797`. |
| 5-minute demo video | Required artifact in the competition guide. | Needs screen recording. |
| PPT file | Required as solution material or defense support. | Draft can be generated locally. |
| Final submission zip | Needed for platform upload. | Generate after docs/PPT are updated. |
| SHA256 sidecar | Helps prove package integrity. | Generate with the zip. |

## Suggested GitHub Repository Structure

```text
README.md
DESIGN.md
EVALUATION.md
SUBMISSION.md
technical_solution.md
testing.md
usage.md
final_report.md
OFFICIAL_TOPIC_ALIGNMENT.md
mooncake_fragmentation_aware_pr_ready_20260703.patch
repro/
logs/
slides/
release/
```

## Suggested Platform Submission Text

```text
This work targets Mooncake topic 2, focusing on Mooncake Store allocation-path
stability for mixed-size KVCache objects. It adds an opt-in
fragmentation_aware allocation strategy that ranks sampled candidate segments
by contiguous-fit capability before aggregate free ratio. The submission
includes a PR-ready Mooncake patch, design and evaluation documents,
reproducible deterministic simulations, validation logs, and demo materials.

Boundary: current evidence is allocation-path simulation and patch
applicability. Full upstream CI, RDMA validation, and real SGLang HiCache
benchmark are not claimed in this initial-round package.
```
