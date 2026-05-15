#!/usr/bin/env pwsh
# ═══════════════════════════════════════════════════════════════════════════════
# Vision Studio — One-click build script
# ═══════════════════════════════════════════════════════════════════════════════

$ErrorActionPreference = "Stop"

# 1. Detect platform
$Preset = if ($IsWindows -or $env:OS -eq "Windows_NT") { "windows-x64" }
          elseif ($IsMacOS) { "macos-arm64" }
          else { "linux-x64" }

Write-Host ">>> Vision Studio Build | Preset: $Preset" -ForegroundColor Cyan

# 2. Bootstrap vcpkg if needed
if (-not (Test-Path "./third-party/vcpkg/vcpkg.exe") -and -not (Test-Path "./third-party/vcpkg/vcpkg")) {
    Write-Host ">>> Bootstrapping vcpkg..." -ForegroundColor Yellow
    if ($IsWindows -or $env:OS -eq "Windows_NT") {
        & ./third-party/vcpkg/bootstrap-vcpkg.bat -disableMetrics
    } else {
        & ./third-party/vcpkg/bootstrap-vcpkg.sh -disableMetrics
    }
}

# 3. Configure
Write-Host ">>> Configuring..." -ForegroundColor Yellow
cmake --preset $Preset

# 4. Build Debug
$PresetPrefix = $Preset.Split('-')[0]
Write-Host ">>> Building Debug..." -ForegroundColor Yellow
cmake --build --preset "$PresetPrefix-debug" --parallel

# 5. Build Release
Write-Host ">>> Building Release..." -ForegroundColor Green
cmake --build --preset "$PresetPrefix-release" --parallel

Write-Host ">>> Done! Check /bin/Debug and /bin/Release" -ForegroundColor White
if ($IsWindows -or $env:OS -eq "Windows_NT") { Pause }
