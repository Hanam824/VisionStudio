# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Vision Studio — a cross-platform C++17 desktop app for image preprocessing and OCR (currently focused on receipt/invoice parsing). Qt Widgets UI + OpenCV preprocessing + a pluggable inference backend (ncnn / ONNX Runtime, both optional and not yet wired to a real model).

## Build commands

vcpkg is bundled as a git submodule at `third-party/vcpkg/` — never rely on `$env:VCPKG_ROOT`. First-time setup:

```powershell
git submodule update --init --recursive
.\third-party\vcpkg\bootstrap-vcpkg.bat -disableMetrics   # Windows
./third-party/vcpkg/bootstrap-vcpkg.sh -disableMetrics    # Linux/macOS
```

Configure + build via CMake presets (`CMakePresets.json`). Configure presets pick the toolchain/generator; build presets pick Debug vs Release on top of a configure preset:

```powershell
# Windows (MSVC / Visual Studio generator, both configs share one configure preset)
cmake --preset windows-x64
cmake --build --preset windows-x64-debug --parallel
cmake --build --preset windows-x64 --parallel          # Release

# Linux (Ninja)
cmake --preset linux-x64-debug   # Debug — separate configure preset
cmake --preset linux-deb-x64     # Release, produces a .deb via CPack
cmake --build --preset linux-x64-debug --parallel
cmake --build --preset linux-deb-x64 --parallel

# macOS (Xcode generator)
cmake --preset macos-arm64   # or macos-x64 for Intel
cmake --build --preset macos-arm64-debug --parallel
cmake --build --preset macos-arm64 --parallel           # Release
```

One-click alternative: `.\build.ps1` (Windows only, configures + builds Release) or `./run_build.sh`. Both scripts create a `cache/` dir for the vcpkg binary cache (safe to delete to reclaim disk space) and will self-bootstrap the vcpkg submodule if missing.

There are also `ci-*` and `*-syntax-check` presets used only by GitHub Actions (`.github/workflows/`) — don't use these for local development.

## Test commands

```powershell
# All GTest unit tests for a given build dir
ctest --test-dir build/windows-x64 --output-on-failure

# A single test (gtest_discover_tests registers each test individually)
ctest --test-dir build/windows-x64 -R ReceiptParserTest.ParsesTotal --output-on-failure

# Python OCR regression benchmark (compares against tests/data/*.expected.txt)
python tests/regression_test.py -e bin/Release/VisionStudio.exe -d tests/data
```

`tests/CMakeLists.txt` globs all `.cpp` in `/tests` into a single `VisionTests` binary and only builds if `find_package(GTest)` succeeds — if GTest isn't resolved by vcpkg, `enable_testing()`/the tests subdirectory is skipped entirely.

## Architecture

**Two-module split, communicating only through an abstract interface:**

```
VisionApp (exe)  --QLibrary/dlopen at runtime-->  VisionCore (shared lib: VisionCore.dll/.so/.dylib)
  Qt Widgets UI                                     IVisionEngine (abstract) <- VisionEngine (impl)
  MainWindow / ImageViewer / LogConsole /            ImageProcessor (OpenCV pipeline)
  ReceiptOcrPanel                                    ReceiptParser (structures OCR output)
                                                      Inference: ncnn / ONNX Runtime (optional, unused by default)
```

- `VisionApp` never includes VisionCore internals — only `VisionCore/IVisionEngine.h` and `VisionCore/Export.h`. It links `VisionCore` at build time for these interface types/symbols, but the actual implementation is loaded dynamically via `QLibrary::resolve("createVisionEngine")`, which returns a C-linkage factory (`CreateEngineFunc`) producing an `IVisionEngine*` wrapped in `std::unique_ptr`. This boundary exists for LGPLv3 compliance (Qt) and to allow alternate engine implementations without touching VisionApp.
- `IVisionEngine` (`src/VisionCore/include/VisionCore/IVisionEngine.h`) is the entire contract: lifecycle (`initialize`/`shutdown`/`isReady`), image I/O (`loadImage`/`getImageData` — BGR buffer), `preprocess(PreprocessOptions)`, `runOcr() -> vector<OcrResult>`, and `setLogCallback(LogCallback)`. `VisionEngine` is the concrete implementation; it's thread-safe (`std::mutex` + `std::atomic_bool`), and its `LogCallback` may fire from any thread — VisionApp marshals it to the UI thread via `QMetaObject::invokeMethod` in `LogConsole`. Internals (`ImageProcessor`, mutex/state) sit behind a Pimpl (`VisionEngine::Impl`) so they never cross the DLL boundary.
- `ImageProcessor` implements a fluent/chaining OpenCV pipeline: `load().toGrayscale().applyThreshold(block, C).correctPerspective()`.
- `ReceiptParser` (`src/VisionCore/include/VisionCore/ReceiptParser.h`) turns raw `OcrResult` vectors into structured `ReceiptData` (merchant, items, totals) and formats it as text or JSON; consumed by `VisionApp`'s `ReceiptOcrPanel`.
- Inference backend availability (`VISIONCORE_HAS_NCNN` / `VISIONCORE_HAS_ONNXRT`) is detected at CMake configure time via `find_package(... QUIET)` in the root `CMakeLists.txt`; `runOcr()` is currently a placeholder — no model files are bundled.
- UI framework is **Qt 6** (`find_package(Qt6 ... COMPONENTS Widgets Core Gui)`, `qtbase` in `vcpkg.json`).

### Directory conventions (enforced by convention, not tooling)

- `/src/<Module>/include/<Module>/*.h` + `/src/<Module>/src/*.cpp` — never put source in the repo root.
- `/build/<preset>/` — CMake intermediates only. `/bin/<Config>/` — final exe/DLL output. `/lib/<Config>/` — static/import libs. Deleting `/build`, `/bin`, `/lib` never touches source.
- `/docs/` — user-facing guides. `/plans/` — architecture/design docs; each file must stay **≤ 200 lines** (split into sub-plans linked from `plans/MASTER_PLAN.md` if it grows past that).
- On Windows, `VisionCore.dll` (or `VisionCored.dll` in Debug — `CMAKE_DEBUG_POSTFIX "d"`) is copied next to `VisionStudio.exe` by a POST_BUILD step; Qt plugin subfolders (`platforms/`, `styles/`, `imageformats/`) are likewise deployed next to the exe. On Linux/macOS this is handled via `RPATH` (`$ORIGIN` / `@executable_path`).

### Code conventions

C++17, `namespace vision { }`, `#pragma once` guards, `PascalCase.h`/`.cpp` files, `PascalCase` classes, `camelCase` methods, `m_camelCase` members. Smart pointers only (no raw `new`/`delete`), prefer `[[nodiscard]]`/`constexpr`/`std::optional`, no C-style casts. No platform-specific APIs without `#ifdef _WIN32` / `#ifdef __APPLE__` guards — use Qt or standard C++ instead.

**Function comments use Doxygen.** Use `/** ... */` blocks with `@brief`, `@param`, and `@return` for functions in headers (and non-trivial private helpers). Simple one-line member/field annotations may still use `///` or trailing `///<`. Section banners (`// ── Name ──`) stay as plain `//` comments — they're navigation aids, not documentation.

```cpp
/**
 * @brief Push parsed receipt data into the metadata fields, items table, and totals.
 * @param receiptData Structured receipt data to display.
 */
void applyParsedReceipt(const vision::ReceiptData& receiptData);
```
