# Build Guide — Vision Studio

> Detailed build instructions for Windows, macOS, and Linux.

---

## Prerequisites (All Platforms)

| Tool | Version | Notes |
|------|---------|-------|
| **Git** | 2.30+ | With submodule support |
| **CMake** | 3.21+ | Must be on PATH |
| **C++17 compiler** | See below | Platform-specific |

vcpkg is **bundled** as a Git submodule — no manual install or `VCPKG_ROOT` needed.

---

## Windows (Primary Target)

### Compiler

**Visual Studio Community 2022** with the *"Desktop development with C++"* workload.

### Build Steps

```powershell
# 1. Clone with submodules
git clone --recursive https://github.com/Hanam824/VisionStudio.git
cd VisionStudio

# 2. Bootstrap vcpkg (first time only)
.\third-party\vcpkg\bootstrap-vcpkg.bat -disableMetrics

# 3. Option A: One-click build
.\build.ps1

# 3. Option B: Manual CMake
cmake --preset windows-x64
cmake --build --preset windows-debug
cmake --build --preset windows-release
```

### VS 2022 IDE

1. Open → CMake → `CMakeLists.txt`
2. Select **"windows-x64"** preset from the toolbar dropdown
3. Press **F5** to build and run

---

## macOS

### Compiler

**Xcode** (install via App Store or `xcode-select --install`).

### Build Steps

```bash
git clone --recursive https://github.com/Hanam824/VisionStudio.git
cd VisionStudio
./third-party/vcpkg/bootstrap-vcpkg.sh -disableMetrics

cmake --preset macos-arm64
cmake --build --preset macos-debug
cmake --build --preset macos-release
```

---

## Linux

### Compiler

**GCC 9+** or **Clang 10+** with Ninja.

```bash
# Ubuntu/Debian prerequisites
sudo apt install build-essential ninja-build cmake git

git clone --recursive https://github.com/Hanam824/VisionStudio.git
cd VisionStudio
./third-party/vcpkg/bootstrap-vcpkg.sh -disableMetrics

cmake --preset linux-x64
cmake --build --preset linux-debug
cmake --build --preset linux-release
```

---

## Output Locations

| Artifact | Path |
|----------|------|
| Debug executable | `bin/Debug/VisionStudio.exe` |
| Release executable | `bin/Release/VisionStudio.exe` |
| VisionCore Debug | `bin/Debug/VisionCored.dll` |
| VisionCore Release | `bin/Release/VisionCore.dll` |
| Static/import libs | `lib/[Config]/` |
| Build intermediates | `build/[platform]/` |

> Deleting `/build/` removes all build artifacts. Source code is never touched.

---

## CMake Presets Reference

| Preset | Platform | Generator |
|--------|----------|-----------|
| `windows-x64` | Windows | Visual Studio 17 2022 |
| `macos-arm64` | macOS | Xcode |
| `linux-x64` | Linux | Ninja |

Build presets: `windows-debug`, `windows-release`, `macos-debug`, `macos-release`, `linux-debug`, `linux-release`.

---

## Running Tests

```powershell
# GTest unit tests (requires GTest via vcpkg)
ctest --test-dir build/windows-x64 --build-config Debug

# Python regression tests
python tests/regression_test.py -e bin/Release/VisionStudio.exe -d tests/data
```

---

## Cleaning

```powershell
# Remove all build artifacts
Remove-Item -Recurse -Force build, bin, lib
```

The source tree remains untouched.
