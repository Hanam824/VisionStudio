# Build System — Vision Studio

> Detail plan for CMake configuration, vcpkg dependencies, presets, and build scripts.
> Referenced from [MASTER_PLAN.md](MASTER_PLAN.md) §3.

---

## Dependencies (vcpkg.json)

```json
{
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
    "name": "vision-studio",
    "version-semver": "0.1.0",
    "description": "Cross-platform OCR & Image Processing Studio",
    "dependencies": [
        "opencv4",
        "qt5-base",
        "ncnn",
        "gtest"
    ],
    "overrides": [],
    "builtin-baseline": "<pinned-commit-hash>"
}
```

| Package | Purpose | Used By |
|---------|---------|---------|
| `opencv4` | Image loading, preprocessing (grayscale, threshold, perspective) | VisionCore |
| `qt5-base` | Cross-platform UI framework (Widgets, Core, Gui) | VisionApp |
| `ncnn` | Lightweight neural network inference for OCR | VisionCore |
| `gtest` | Unit testing framework | tests/ |

**Future additions**: `onnxruntime` (alternative inference backend), `spdlog` (structured logging).

---

## CMakePresets.json (OBS-Style)

Primary target: **Visual Studio Community 2022 on Windows x64**.
Secondary targets: macOS (Xcode), Linux (Ninja/GCC).

```json
{
    "version": 6,
    "cmakeMinimumRequired": { "major": 3, "minor": 21, "patch": 0 },
    "configurePresets": [
        {
            "name": "default",
            "hidden": true,
            "binaryDir": "${sourceDir}/build/${presetName}",
            "cacheVariables": {
                "CMAKE_TOOLCHAIN_FILE": "${sourceDir}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake",
                "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
            }
        },
        {
            "name": "windows-x64",
            "displayName": "Windows x64 (VS 2022)",
            "inherits": "default",
            "generator": "Visual Studio 17 2022",
            "architecture": { "value": "x64", "strategy": "set" },
            "binaryDir": "${sourceDir}/build/windows-x64",
            "cacheVariables": {
                "VCPKG_TARGET_TRIPLET": "x64-windows"
            },
            "condition": { "type": "equals", "lhs": "${hostSystemName}", "rhs": "Windows" }
        }
    ],
    "buildPresets": [
        {
            "name": "windows-debug",
            "configurePreset": "windows-x64",
            "configuration": "Debug"
        },
        {
            "name": "windows-release",
            "configurePreset": "windows-x64",
            "configuration": "Release"
        }
    ]
}
```

> **Key decision**: `CMAKE_TOOLCHAIN_FILE` points to `${sourceDir}/third-party/vcpkg/...` (bundled), NOT `$env{VCPKG_ROOT}`. This guarantees any PC can build without environment setup.

---

## Root CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.21)
project(VisionStudio VERSION 0.1.0 LANGUAGES CXX
        DESCRIPTION "Cross-platform OCR & Image Processing Studio")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "d")    # Debug DLLs: VisionCored.dll

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/$<CONFIG>")

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(OpenCV 4 REQUIRED COMPONENTS core imgproc imgcodecs)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(ncnn QUIET)
find_package(onnxruntime QUIET)
find_package(GTest QUIET)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Sub-projects
add_subdirectory(src/VisionCore)
add_subdirectory(src/VisionApp)

# Tests (only if GTest found)
if(GTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()
```

---

## RPATH for macOS/Linux

```cmake
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "@executable_path/.")
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/.")
endif()
```

---

## build.ps1 — One-Click Build Script

```powershell
#!/usr/bin/env pwsh
# Vision Studio — One-click build script

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
Write-Host ">>> Building Debug..." -ForegroundColor Yellow
cmake --build --preset "$Preset" --config Debug --parallel

# 5. Build Release
Write-Host ">>> Building Release..." -ForegroundColor Green
cmake --build --preset "$Preset" --config Release --parallel

Write-Host ">>> Done! Check /bin/Debug and /bin/Release" -ForegroundColor White
if ($IsWindows -or $env:OS -eq "Windows_NT") { Pause }
```

---

## Output File Naming

| Config | Executable | Core DLL | Core Lib |
|--------|-----------|----------|----------|
| Debug | `VisionStudio.exe` | `VisionCored.dll` | `VisionCored.lib` |
| Release | `VisionStudio.exe` | `VisionCore.dll` | `VisionCore.lib` |

The `d` suffix on Debug builds comes from `CMAKE_DEBUG_POSTFIX "d"`.
