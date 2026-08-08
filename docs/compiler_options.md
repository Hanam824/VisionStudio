# C++ Compiler Toolchain Options on Windows

This document summarizes a discussion about compiling Vision Studio on Windows, specifically addressing devices with limited storage (like a Surface Go) and the role of build systems.

## The Role of Ninja vs. The Compiler

It is important to understand the difference between a build system and a compiler:

* **The Compiler (MSVC, GCC, Clang):** The actual engine that translates C++ code (`.cpp` files) into machine code (`.exe` or `.dll`).
* **CMake:** The architect that reads `CMakeLists.txt` and generates a master build plan.
* **Ninja:** The build orchestrator (manager). It reads CMake's plan and commands the compiler to build files in the most efficient, parallel order possible.

**Important:** Ninja is *not* a compiler. If you run a Ninja build without a C++ toolchain installed, the build will fail completely (e.g., `CMAKE_CXX_COMPILER not set` and `Unable to find a valid Visual Studio instance`) because Ninja has no compiler to execute the translation.

## Windows Toolchain Comparison

When choosing a toolchain on Windows, there is a trade-off between **disk space (lightweight)** and **compilation speed / compatibility (faster)**. 

### 1. Microsoft Visual C++ Build Tools (MSVC) — *The Standard & Recommended*
* **Size:** ~4 GB to 5 GB
* **Speed:** Very fast compilation and execution. Heavily optimized for Windows. 
* **Compatibility:** 100% native. `vcpkg`, CMake, and almost every C++ library are designed to work perfectly with MSVC on Windows without any extra configuration. (Uses the `x64-windows` vcpkg triplet).
* **Installation:** Can be installed via `winget`:
  ```powershell
  winget install --id Microsoft.VisualStudio.2022.BuildTools --exact --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
  ```

### 2. MinGW-w64 (GCC for Windows) — *The Most Lightweight*
* **Size:** ~1 GB
* **Speed:** Good execution speed, but the compilation process itself is actually slightly *slower* on Windows compared to MSVC.
* **Compatibility:** Because it is not native to Windows, integrating it with `vcpkg` requires changing your triplet to `x64-mingw-dynamic`. This can sometimes result in compatibility headaches where certain C++ libraries fail to compile.

### 3. LLVM / Clang — *The Fastest Compiler*
* **Size:** ~1.5 GB to 2 GB
* **Speed:** Incredibly fast compilation times and excellent optimization. 
* **Compatibility:** On Windows, Clang relies heavily on the Windows standard libraries... which are provided by the MSVC Build Tools! To use Clang properly on Windows, you usually end up having to install the MSVC Build Tools anyway. 

## Conclusion

If disk space is strictly limited (e.g., `< 4GB` available), **MinGW-w64** is the only viable option, though it requires extra configuration. 

However, if you have 4-5 GB to spare, the **Microsoft Build Tools** provide a significantly faster, headache-free experience out of the box.
