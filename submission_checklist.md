# Submission Checklist

- [x] Official CCF notice saved.
- [x] GitLink competition/task data saved.
- [x] Candidate decision matrix created.
- [x] Main task selected.
- [x] Official repositories cloned.
- [x] Feature implementation drafted.
- [x] Unit test drafted.
- [x] Documentation drafted.
- [x] Build configured.
- [x] Lightweight allocation strategy unit test passed.
- [x] Standalone fragmentation reproduction passed.
- [x] Build/log artifacts saved.
- [x] Final patch exported.
- [x] Demo video script finalized.

## GitLink Submission Items

- Source code or PR link.
- Technical solution document or slides.
- 5-minute demo video.
- Test/benchmark result logs.
- GitHub open-source repository link.

## Current Risk

Full Mooncake `allocation_strategy_test` binary did not finish in this local WSL Ubuntu 20.04 / GCC10 / Windows-mounted workspace before timeout. The patch includes local build compatibility fixes and the core strategy has passed a header-level unit test plus deterministic fragmentation simulation. A cleaner final validation should use a native Linux workspace with newer GCC/libstdc++ or the Mooncake CI image.
