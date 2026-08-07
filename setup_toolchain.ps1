$ErrorActionPreference = "Stop"

$ToolsDir = "$PSScriptRoot\.tools"
$CacheDir = "$PSScriptRoot\.cache"

$MinGwUrl = "https://github.com/brechtsanders/winlibs_mingw/releases/download/14.2.0posix-18.1.8-12.0.0-msvcrt-r1/winlibs-x86_64-posix-seh-gcc-14.2.0-mingw-w64msvcrt-12.0.0-r1.zip"
$NinjaUrl = "https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-win.zip"

$MinGwZip = "$CacheDir\mingw.zip"
$NinjaZip = "$CacheDir\ninja.zip"

# 1. Quick check: Is it already fully installed?
if (Test-Path "$ToolsDir\mingw64\bin\gcc.exe") {
    Write-Host "Toolchain already installed in .tools! Skipping download." -ForegroundColor Green
    exit 0
}

# Create directories if they don't exist
if (-Not (Test-Path $ToolsDir)) { New-Item -ItemType Directory -Path $ToolsDir | Out-Null }
if (-Not (Test-Path $CacheDir)) { New-Item -ItemType Directory -Path $CacheDir | Out-Null }

# 2. Handle MinGW Download (Using Cache)
Write-Host "1/3 Checking MinGW-w64 (GCC)..." -ForegroundColor Cyan
if (-Not (Test-Path $MinGwZip)) {
    Write-Host "  -> Downloading MinGW to cache..." -ForegroundColor Yellow
    Invoke-WebRequest -Uri $MinGwUrl -OutFile $MinGwZip
} else {
    Write-Host "  -> MinGW found in cache, skipping download." -ForegroundColor Green
}
Write-Host "  -> Extracting MinGW..." -ForegroundColor Cyan
Expand-Archive -Path $MinGwZip -DestinationPath $ToolsDir -Force

# 3. Handle Ninja Download (Using Cache)
Write-Host "2/3 Checking Ninja..." -ForegroundColor Cyan
if (-Not (Test-Path $NinjaZip)) {
    Write-Host "  -> Downloading Ninja to cache..." -ForegroundColor Yellow
    Invoke-WebRequest -Uri $NinjaUrl -OutFile $NinjaZip
} else {
    Write-Host "  -> Ninja found in cache, skipping download." -ForegroundColor Green
}
Write-Host "  -> Extracting Ninja..." -ForegroundColor Cyan
Expand-Archive -Path $NinjaZip -DestinationPath "$ToolsDir\mingw64\bin" -Force

Write-Host "3/3 Setup Complete!" -ForegroundColor Green
