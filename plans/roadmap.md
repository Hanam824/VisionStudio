# Development Roadmap — Vision Studio

> Phase tracking and future plans.
> Referenced from [MASTER_PLAN.md](MASTER_PLAN.md) §6.

---

## Phase 1 — Core Interface & Setup ✅
- [x] Define `IVisionEngine` abstract interface
- [x] Implement `Export.h` with cross-platform DLL macros
- [x] Configure `vcpkg.json` manifest
- [x] Set up `CMakePresets.json` (OBS-style)
- [x] Root `CMakeLists.txt` with output directory logic
- [x] `.gitignore` for build artifacts

## Phase 2 — VisionCore Implementation ✅
- [x] `ImageProcessor` — OpenCV preprocessing pipeline
- [x] `VisionEngine` — Concrete `IVisionEngine` implementation
- [x] `createVisionEngine()` C-linkage factory function
- [x] Conditional ncnn / ONNX Runtime linking

## Phase 3 — VisionApp (Qt UI) ✅
- [x] `MainWindow` — toolbar, menus, dock widgets
- [x] `ImageViewer` — image display with zoom/pan
- [x] `LogConsole` — real-time log from engine
- [x] `main.cpp` — entry point with dark theme loading
- [x] Dynamic loading of VisionCore via QLibrary

## Phase 4 — Testing ✅
- [x] GTest unit tests for `ImageProcessor`
- [x] Python regression script (`regression_test.py`)
- [x] Test data directory with `.expected.txt` convention

## Phase 5 — Polish & Hardening (In Progress)
- [x] Add `CMAKE_DEBUG_POSTFIX "d"` to `VisionCore` target properties
- [x] Bundle vcpkg as Git submodule (`git submodule add`)
- [x] Update `CMakePresets.json` to use `${sourceDir}/third-party/vcpkg/` toolchain
- [x] Create `build.ps1` and `run_build.bat`
- [x] Add RPATH settings for macOS/Linux in root `CMakeLists.txt`
- [x] Add installer/packaging (CPack: NSIS on Windows, DMG on macOS)
- [x] Add CI/CD pipeline (GitHub Actions)
- [ ] Expand dark theme with custom icons and styling
- [ ] Add batch/CLI mode for headless OCR processing
- [x] Create `/docs/getting_started.md` — beginner guide (clone → build → run)
- [x] Create `/docs/architecture.md` — system design for new contributors
- [x] Create `/docs/build_guide.md` — detailed build instructions per platform
- [x] Create `/docs/contributing.md` — code style, PR process, naming conventions
- [x] Create `/docs/faq.md` — common errors & troubleshooting

## Phase 6 — Future Roadmap
- [ ] GPU acceleration via ONNX Runtime CUDA/DirectML providers
- [ ] Multi-language OCR model support
- [ ] Table structure recognition
- [ ] PDF import/export
- [ ] Plugin system for custom `IVisionEngine` implementations
- [ ] macOS `.app` bundle and Linux `.AppImage` packaging
