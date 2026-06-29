#!/usr/bin/env pwsh
# ═══════════════════════════════════════════════════════════════════════════════
# Vision Studio — One-click build script
# ═══════════════════════════════════════════════════════════════════════════════

$ErrorActionPreference = "Stop"

# 1. Detect platform
if (-not ($IsWindows -or $env:OS -eq "Windows_NT")) {
    Write-Error "This script is for Windows only. Please use run_build.sh for macOS/Linux."
    exit 1
}

$Preset = "windows-x64"

Write-Host ">>> Vision Studio Build | Preset: $Preset" -ForegroundColor Cyan

# Set up local vcpkg binary cache
$CachePath = Join-Path $PWD "cache"
if (-not (Test-Path $CachePath)) { New-Item -ItemType Directory -Path $CachePath | Out-Null }
$env:VCPKG_BINARY_SOURCES = "clear;files,$CachePath,readwrite"

# 2. Check and initialize vcpkg submodule if empty
if (-not (Test-Path "./third-party/vcpkg/bootstrap-vcpkg.bat")) {
    Write-Host ">>> vcpkg submodule is empty! Initializing git submodules..." -ForegroundColor Yellow
    git submodule update --init --recursive
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to initialize git submodules. Are you inside a valid git repository?"
        exit 1
    }
}

# 3. Bootstrap vcpkg if needed
if (-not (Test-Path "./third-party/vcpkg/vcpkg.exe")) {
    Write-Host ">>> Bootstrapping vcpkg..." -ForegroundColor Yellow
    & ./third-party/vcpkg/bootstrap-vcpkg.bat -disableMetrics
}

# 4. Configure
Write-Host ">>> Configuring..." -ForegroundColor Yellow
cmake --preset $Preset

# 5. Build Debug
# Write-Host ">>> Building Debug..." -ForegroundColor Yellow
# cmake --build --preset "$Preset-debug" --parallel

# 6. Build Release
Write-Host ">>> Building Release..." -ForegroundColor Green
cmake --build --preset "$Preset" --parallel

Write-Host ">>> Done! Check /bin/Debug and /bin/Release" -ForegroundColor White
Pause
