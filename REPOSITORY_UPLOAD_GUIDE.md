# Repository Upload Guide

Suggested repository name: `ccf-mooncake-fragmentation-aware`

Use this directory as the source tree for the Mooncake KVCache storage design and performance optimization submission. The directory contains the patch, reproduction programs, logs, final check report, and submission documents extracted from the verified package.

Suggested first commit message:

```text
Add Mooncake fragmentation-aware allocation submission
```

Suggested push flow after creating the remote repository:

```powershell
git remote add origin <REMOTE_URL>
git push -u origin main
```

Important limitation to keep visible in the repository description and submission form: the lightweight simulation and available logs are included, but the full Mooncake Store build, `allocation_strategy_test`, and benchmark path still need native Linux or CI-like validation.
