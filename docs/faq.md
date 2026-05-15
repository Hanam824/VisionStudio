# FAQ — Vision Studio

> Common errors, troubleshooting, and frequently asked questions.

---

## Build Issues

### Q: CMake says "vcpkg toolchain file not found"

**A:** The vcpkg submodule hasn't been cloned. Run:
```powershell
git submodule update --init --recursive
.\third-party\vcpkg\bootstrap-vcpkg.bat -disableMetrics
```

### Q: vcpkg packages fail to install

**A:** Ensure you have the Visual Studio 2022 *"Desktop development with C++"* workload installed. vcpkg needs the MSVC compiler and Windows SDK.

### Q: CMake can't find OpenCV or Qt5

**A:** These are installed automatically by vcpkg during the first configure. The initial build may take 15-30 minutes while vcpkg compiles dependencies. Let it finish.

### Q: "Generator not found: Visual Studio 17 2022"

**A:** Install Visual Studio 2022 (Community edition is free). Alternatively, use a different preset (`linux-x64` with Ninja, `macos-arm64` with Xcode).

---

## Runtime Issues

### Q: "VisionCore.dll not found" when running VisionStudio.exe

**A:** The DLL must be in the same directory as the executable. The CMake build copies it automatically via a POST_BUILD step. If running manually, ensure both files are in `bin/[Config]/`.

### Q: The application starts but shows "Warning: VisionCore library not found"

**A:** This means `QLibrary` couldn't load `VisionCore.dll`. Check:
1. The DLL exists in the same folder as the `.exe`.
2. All OpenCV DLLs are also present (vcpkg should handle this).
3. You're running the correct Config (Debug with `VisionCored.dll`, Release with `VisionCore.dll`).

### Q: Preprocessing works but OCR returns no results

**A:** OCR inference requires ncnn or ONNX Runtime model files, which are not yet bundled with the project. The preprocessing pipeline (grayscale, threshold, perspective correction) works immediately.

---

## Development Issues

### Q: Where do I add new source files?

**A:** Under `/src/[Module]/`:
- Headers in `include/[ModuleName]/*.h`
- Sources in `src/*.cpp`

Never place source files in the project root.

### Q: How do I run tests?

**A:**
```powershell
# GTest unit tests
ctest --test-dir build/windows-x64 --build-config Debug

# Python regression tests (when OCR is available)
python tests/regression_test.py -e bin/Release/VisionStudio.exe -d tests/data
```

### Q: Can I use a different IDE?

**A:** Yes. `CMakePresets.json` is IDE-agnostic. CLion, VS Code (with CMake Tools), and Qt Creator all support CMake presets. Select the appropriate preset for your platform.

---

## Platform-Specific

### Q: How do I build on macOS?

**A:**
```bash
cmake --preset macos-arm64
cmake --build --preset macos-debug
```
Requires Xcode and its command-line tools.

### Q: How do I build on Linux?

**A:**
```bash
sudo apt install build-essential ninja-build cmake
cmake --preset linux-x64
cmake --build --preset linux-debug
```

---

## Still Stuck?

Open an issue on [GitHub](https://github.com/Hanam824/VisionStudio/issues) with:
1. Your OS and compiler version
2. The full CMake error output
3. Steps to reproduce the problem
