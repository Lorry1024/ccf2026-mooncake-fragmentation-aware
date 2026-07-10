param(
    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$source = Join-Path $repoRoot 'repro\topic_aligned_store_scalability_sim.cpp'
$buildDir = Join-Path $env:TEMP 'ccf2026-mooncake-demo'
$executable = Join-Path $buildDir 'topic_aligned_store_scalability_sim.exe'

if (-not $SkipBuild) {
    $compiler = Get-Command 'g++' -ErrorAction SilentlyContinue
    if (-not $compiler) {
        throw 'g++ was not found. Install MinGW-w64 and add its bin directory to PATH.'
    }

    New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    Write-Host 'Building deterministic Mooncake allocation simulation...' -ForegroundColor Cyan
    & $compiler.Source '-std=c++17' '-O2' $source '-o' $executable
    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed with exit code $LASTEXITCODE."
    }
}

if (-not (Test-Path -LiteralPath $executable)) {
    throw "Demo executable not found at $executable. Run without -SkipBuild first."
}

Write-Host 'Running fragmentation-aware allocation comparison...' -ForegroundColor Cyan
$previousErrorActionPreference = $ErrorActionPreference
try {
    # Windows PowerShell wraps native stderr lines as ErrorRecord objects.
    # Capture them with Continue, then print their clean string representation.
    $ErrorActionPreference = 'Continue'
    $rawOutput = & $executable 2>&1
    $exitCode = $LASTEXITCODE
} finally {
    $ErrorActionPreference = $previousErrorActionPreference
}
$lines = @($rawOutput | ForEach-Object { $_.ToString() })

$stableSummaryPattern =
    '^(SUMMARY|' +
    'free_ratio_primary_success=|' +
    'fragmentation_aware_primary_success=|' +
    'free_ratio_eventual_success=|' +
    'fragmentation_aware_eventual_success=|' +
    'free_ratio_fallback_attempts=|' +
    'fragmentation_aware_fallback_attempts=|' +
    'avg_candidates_scored_free_ratio=|' +
    'avg_candidates_scored_fragmentation_aware=|' +
    'NOTE:|PASS )'

$summary = @($lines | Where-Object { $_ -match $stableSummaryPattern })
$summary | ForEach-Object { Write-Host $_ }

$expected = @(
    'free_ratio_primary_success=0/6',
    'fragmentation_aware_primary_success=6/6',
    'free_ratio_fallback_attempts=11',
    'fragmentation_aware_fallback_attempts=0',
    'avg_candidates_scored_free_ratio=5.00',
    'avg_candidates_scored_fragmentation_aware=5.00',
    'PASS topic_aligned_store_scalability_sim'
)
$missing = @($expected | Where-Object { $_ -notin $summary })

if ($exitCode -ne 0 -or $missing.Count -gt 0) {
    if ($missing.Count -gt 0) {
        Write-Host ('Missing expected evidence: ' + ($missing -join ', ')) -ForegroundColor Red
    }
    throw "PowerShell demo validation failed with exit code $exitCode."
}

Write-Host 'PowerShell demo passed.' -ForegroundColor Green
