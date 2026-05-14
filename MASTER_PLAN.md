Project Name: Vision Studio (Cross-platform OCR & Image Processing)
Tech Stack: C++17, Qt 5.15, OpenCV 4, ncnn/ONNX, vcpkg, CMake.

1. Project Architecture & Standards (MUST FOLLOW):

Build Logic: Follows OBS Studio's CMakePresets.json strategy.

Folder Structure:

/source: Clean C++ source code.

/build/[platform]: CMake/IDE metadata + /temp folder for all intermediate files (.obj, .pdb).

/bin/[Config]: Final binaries (.exe, .dll, .app).

/lib/[Config]: Linker files.

Encapsulation: All AI/Image logic must be in VisionCore (Shared Library). UI (VisionApp) must interact via Abstract Interfaces (OOP) to comply with LGPLv3.

2. Development Workflow (Step-by-Step):

PHASE 1: Core Interface & Setup

Define IVisionEngine in VisionCore to handle OCR and Pre-processing.

Implement DLL export/import macros for cross-platform compatibility (Windows MSVC vs. Apple Clang).

Configure vcpkg manifest to pull dependencies automatically.

PHASE 2: VisionCore Implementation (AI & OpenCV)

Preprocessing: Grayscale, Thresholding, Perspective Correction using OpenCV.

Inference: Integrate ncnn or ONNX Runtime for OCR tasks.

Optimization: Use Multi-threading for frame processing to keep UI responsive.

PHASE 3: VisionApp (Qt UI)

Design a modern dark-themed UI.

Implement dynamic loading of VisionCore DLL/dylib.

Create a "Log Console" that reads metadata from the background engine.

PHASE 4: Testing & Quality Control

Unit Tests: Use GTest to verify image processing functions.

Regression Tests: A Python script to compare OCR output accuracy from /tests/data.

3. Specific Constraints for AI assistant:

Strict Pathing: Never suggest putting files in the root. Follow the /source, /build, /bin hierarchy.

Zero-Junk Build: Always ensure CMake directs intermediate objects to build/[platform]/temp/$<CONFIG>.

Cross-platform Safety: Avoid Win32-only or Mac-only APIs. Use Qt/Standard C++ wrappers. If platform-specific code is needed, wrap it in #ifdef _WIN32 or #ifdef __APPLE__.

Modern C++: Prefer smart pointers (std::unique_ptr, std::shared_ptr) and RAII.

4. Initial Task:
Analyze the project structure and suggest the header file for VisionCore/IVisionEngine.h that includes proper export macros for Windows/macOS.