cmake_minimum_required(VERSION 3.21)

# ── Project ──────────────────────────────────────────────────────────────────
project(VisionStudio
    VERSION   0.1.0
    LANGUAGES CXX
    DESCRIPTION "Cross-platform OCR & Image Processing Studio"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Output Directories ──────────────────────────────────────────────────────
# Final binaries go to /bin/[Config], linker files to /lib/[Config].
# Intermediate objects are handled by CMakePresets (build/[platform]/temp).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/$<CONFIG>")

# On Windows, put DLLs next to the executable so they are found at runtime.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/lib/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/lib/Release")

# ── Global Compiler Options ─────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Dependencies (via vcpkg) ────────────────────────────────────────────────
find_package(OpenCV  4 REQUIRED COMPONENTS core imgproc imgcodecs)
find_package(Qt5     REQUIRED COMPONENTS Widgets Core Gui)

# Optional: ncnn or ONNX Runtime for inference
find_package(ncnn QUIET)
find_package(onnxruntime QUIET)

# Optional: GTest for testing
find_package(GTest QUIET)

# Enable Qt automatic processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ── Sub-projects ────────────────────────────────────────────────────────────
add_subdirectory(source/VisionCore)
add_subdirectory(source/VisionApp)

# ── Tests ────────────────────────────────────────────────────────────────────
if(GTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()
