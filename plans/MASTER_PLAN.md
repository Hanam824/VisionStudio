# MASTER PLAN — Vision Studio

> **Project**: Cross-platform OCR & Image Processing Studio
> **Tech Stack**: C++17 · Qt 5.15 · OpenCV 4 · ncnn / ONNX Runtime · vcpkg · CMake
> **License**: LGPLv3 · **Version**: 0.1.0
> **Primary IDE**: Visual Studio Community 2022 (Windows)

---

## 1. Non-Negotiable Rules

These rules apply to **every file, every commit, every AI-assisted edit**.

### 1.1 Zero-Junk Source Tree

| Zone | Path | Contents |
|------|------|----------|
| **Source** | `/src/` | Clean C++ source code only. No generated files. |
| **Build** | `/build/[platform]/` | CMake/IDE metadata + all intermediate files (`.obj`, `.pdb`). |
| **Output** | `/bin/[Config]/` | Final executables and DLLs. |
| **Linker** | `/lib/[Config]/` | Static/import libraries (`.lib`, `.a`). |
| **Docs** | `/docs/` | User-facing documentation, beginner guides, tutorials. |
| **Plans** | `/plans/` | All project plans, implementation plans, walkthroughs. |

> Deleting `/build/` removes all build artifacts. Source code is never touched.

### 1.2 Bundled vcpkg (Portable Across PCs)
- vcpkg lives as a **Git submodule** at `third-party/vcpkg/`. Any machine builds after `git clone --recursive`.
- **No dependency on `$env{VCPKG_ROOT}`**.
- Toolchain: `${sourceDir}/third-party/vcpkg/scripts/buildsystems/vcpkg.cmake`

### 1.3 OBS Studio Build Strategy
`CMakePresets.json` defines all platform/config combinations.
In VS 2022: **select preset from toolbar → press F5**.

### 1.4 Strict Pathing
- **Never** place source files in the root directory.
- **Never** create files outside `/src`, `/build`, `/bin`, `/lib`, `/tests`, `/docs`, `/plans`.
- **Always** follow `include/[ModuleName]/*.h` + `src/*.cpp` convention.

### 1.5 Plan Document Rules
- Every plan file in `/plans/` must be **≤ 200 lines**.
- If a plan grows beyond 200 lines, **split it** into focused sub-plans and link from `MASTER_PLAN.md`.
- `MASTER_PLAN.md` is the overview index — keep it concise, link to details.

### 1.6 Modern C++17
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`), RAII, no raw `new`/`delete`.
- `[[nodiscard]]`, `constexpr`, `std::optional`. No C-style casts.

### 1.7 Cross-Platform Safety
- No Win32-only or Mac-only APIs without `#ifdef _WIN32` / `#ifdef __APPLE__` guards.
- Use Qt / Standard C++ wrappers.

---

## 2. Architecture Overview

```
 VisionApp (EXE)  ──QLibrary──▶  VisionCore (DLL)
   Qt 5 UI                        IVisionEngine (interface)
   Dark Theme                     ImageProcessor (OpenCV)
   Log Console                    Inference (ncnn/ONNX)
```

**Key principles:**
- VisionApp loads VisionCore **at runtime** via `QLibrary` / `dlopen`.
- Communication is **only** through the `IVisionEngine` abstract interface.
- `createVisionEngine()` — C-linkage factory function for dynamic loading.
- **LGPLv3 compliance**: DLL boundary keeps AI/image logic separate from Qt UI.

> 📄 **Full details**: [plans/architecture.md](architecture.md) — interface code, export macros, dynamic loading pattern, data structures.

---

## 3. Folder Structure

```
VisionStudio/
├── CMakeLists.txt                # Root orchestrator
├── CMakePresets.json              # OBS-style platform presets
├── vcpkg.json                    # Dependency manifest
├── third-party/vcpkg/            # Git submodule (bundled)
├── build.ps1 / run_build.bat     # One-click build scripts
├── LICENSE · .gitignore · .gitmodules
│
├── docs/                         # User-facing documentation
│   ├── README.md                 #   Project overview
│   ├── getting_started.md        #   Beginner guide
│   ├── architecture.md           #   System design
│   ├── build_guide.md            #   Build instructions
│   ├── contributing.md           #   Code style & PR process
│   └── faq.md                    #   Troubleshooting
│
├── plans/                        # Project planning
│   ├── MASTER_PLAN.md            #   ★ This file
│   ├── architecture.md           #   Architecture details
│   ├── build_system.md           #   Build system details
│   ├── ocr_pipeline.md           #   OCR & UI pipeline
│   ├── testing_plan.md           #   Testing strategy
│   ├── roadmap.md                #   Phase tracking
│   ├── implementation_plan.md    #   Current sprint
│   └── walkthrough.md            #   Post-implementation summary
│
├── src/
│   ├── VisionCore/               # Shared Library (DLL)
│   │   ├── include/VisionCore/   #   Export.h, IVisionEngine.h,
│   │   │                         #   VisionEngine.h, ImageProcessor.h
│   │   └── src/                  #   VisionEngine.cpp, ImageProcessor.cpp
│   └── VisionApp/                # Qt Executable
│       ├── include/VisionApp/    #   MainWindow.h, ImageViewer.h, LogConsole.h
│       ├── src/                  #   main.cpp, MainWindow.cpp, etc.
│       └── resources/            #   app.qrc, styles/dark_theme.qss
│
├── tests/                        # Test infrastructure
│   ├── test_image_processor.cpp  #   GTest unit tests
│   ├── regression_test.py        #   Python OCR accuracy benchmark
│   └── data/                     #   sample.png + sample.expected.txt
│
├── build/ · bin/ · lib/          # Generated (git-ignored)
```

---

## 4. Sub-Plans Index

| Plan | Description | Lines |
|------|-------------|-------|
| 📐 [architecture.md](architecture.md) | Interface design, export macros, dynamic loading, data structs | ~110 |
| 🔧 [build_system.md](build_system.md) | CMake, vcpkg, presets, build scripts, output naming | ~170 |
| 🤖 [ocr_pipeline.md](ocr_pipeline.md) | Preprocessing pipeline, inference backends, UI layout | ~80 |
| 🧪 [testing_plan.md](testing_plan.md) | GTest, Python regression, test data conventions | ~45 |
| 🗺️ [roadmap.md](roadmap.md) | Phase 1–6 progress tracking, future features | ~60 |
| 📋 [implementation_plan.md](implementation_plan.md) | Current sprint task breakdown | — |
| 📝 [walkthrough.md](walkthrough.md) | Post-implementation summary | — |

---

## 5. Versioning & Code Style

| Rule | Convention |
|------|-----------|
| Version scheme | Semantic: `MAJOR.MINOR.PATCH` (currently `0.1.0`) |
| Namespace | `namespace vision { }` |
| Header guards | `#pragma once` |
| File naming | `PascalCase.h` / `PascalCase.cpp` |
| Class naming | `PascalCase` |
| Method naming | `camelCase` |
| Member variables | `m_camelCase` |

---

## 6. Developer Quick Start

```powershell
# 1. Clone with submodules
git clone --recursive https://github.com/Hanam824/VisionStudio.git
cd VisionStudio

# 2. Bootstrap vcpkg (first time only)
.\third-party\vcpkg\bootstrap-vcpkg.bat -disableMetrics

# 3. Open in VS 2022: File → Open → CMake → CMakeLists.txt
#    Select "windows-x64" preset → F5

# --- OR ---
.\build.ps1
```

---

## 7. AI Assistant Constraints

1. **Strict Pathing**: Follow `/src`, `/build`, `/bin`, `/docs`, `/plans` hierarchy.
2. **Zero-Junk Build**: All intermediates go to `build/[platform]/`.
3. **Cross-platform**: No platform-only APIs without `#ifdef` guards.
4. **Modern C++17**: Smart pointers, RAII, `[[nodiscard]]`, no raw `new`/`delete`.
5. **Interface Boundary**: VisionApp must NEVER `#include` VisionCore internals. Only `IVisionEngine.h` + `Export.h`.
6. **vcpkg Bundled**: Always `${sourceDir}/third-party/vcpkg/`, never `$env{VCPKG_ROOT}`.
7. **Naming**: PascalCase files / camelCase methods / `m_` members.
8. **Comments**: Preserve all existing comments unrelated to the change.
9. **Documentation**: User docs → `/docs/`. Plans & designs → `/plans/`.
10. **Plan Size Limit**: Every file in `/plans/` must be **≤ 200 lines**. If a plan grows beyond this, split it into focused sub-plans and link from `MASTER_PLAN.md`.