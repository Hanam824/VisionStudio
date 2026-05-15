# Getting Started — Vision Studio

> From zero to running in 5 minutes.

---

## Prerequisites

| Tool | Minimum Version | Download |
|------|----------------|----------|
| **Git** | 2.30+ | [git-scm.com](https://git-scm.com) |
| **CMake** | 3.21+ | [cmake.org](https://cmake.org/download) |
| **Visual Studio 2022** | Community (free) | [visualstudio.microsoft.com](https://visualstudio.microsoft.com) |

> **VS 2022 Workload**: Install *"Desktop development with C++"*. No other workloads are needed.

---

## 1. Clone the Repository

```powershell
git clone --recursive https://github.com/Hanam824/VisionStudio.git
cd VisionStudio
```

The `--recursive` flag pulls the **bundled vcpkg** submodule automatically. No `VCPKG_ROOT` environment variable is needed.

---

## 2. Bootstrap vcpkg (First Time Only)

```powershell
.\third-party\vcpkg\bootstrap-vcpkg.bat -disableMetrics
```

This compiles the vcpkg binary. It only needs to run once.

---

## 3. Open in Visual Studio

1. **File → Open → CMake...** → select `CMakeLists.txt` in the repo root.
2. VS 2022 auto-detects `CMakePresets.json` and shows the **"windows-x64"** preset in the toolbar.
3. Press **F5** to configure, build, and run.

### Or: Command-Line Build

```powershell
# One-click (builds both Debug and Release)
.\build.ps1

# Or manual steps:
cmake --preset windows-x64
cmake --build --preset windows-debug
```

---

## 4. Run

After building, the executable is at:

| Config | Path |
|--------|------|
| Debug | `bin/Debug/VisionStudio.exe` |
| Release | `bin/Release/VisionStudio.exe` |

Double-click or run from terminal. The dark-themed UI will open with:
- **Toolbar**: Open · Preprocess · Run OCR
- **Central**: Image Viewer with zoom (scroll) and pan (middle-click)
- **Bottom**: Dockable Log Console

---

## 5. Quick Test

1. Click **Open** (or `Ctrl+O`) and select any image (PNG, JPG, BMP, TIFF).
2. Click **Preprocess** (or `Ctrl+P`) to convert to grayscale + threshold.
3. Click **Run OCR** (or `Ctrl+R`) — results appear in the Log Console.

> **Note**: OCR inference requires ncnn/ONNX model files (not yet bundled). The preprocessing pipeline works immediately.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| `vcpkg` packages fail to build | Ensure VS 2022 C++ workload is installed. Run `.\third-party\vcpkg\bootstrap-vcpkg.bat` again. |
| CMake can't find Qt5 | vcpkg installs Qt automatically — wait for the first configure to finish. |
| "VisionCore.dll not found" at runtime | Ensure both `.exe` and `.dll` are in `bin/[Config]/`. The CMake build does this automatically. |

See [FAQ](faq.md) for more.
