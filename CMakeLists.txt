cmake_minimum_required(VERSION 3.21)

# ── Project ──────────────────────────────────────────────────────────────────
project(VisionStudio
    VERSION   0.1.0
    LANGUAGES CXX
    DESCRIPTION "Cross-platform OCR & Image Processing Studio"
)

# Limit configurations to Debug and Release for IDEs (makes .sln / Xcode simpler for beginners)
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported IDE configurations" FORCE)
endif()

# Default to Release for single-config generators (like Ninja/Make) if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "d")    # Debug DLLs: VisionCored.dll

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

# ── RPATH (macOS / Linux) ───────────────────────────────────────────────────
# Ensure the executable finds VisionCore alongside itself at runtime.
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "@executable_path/.")
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/.")
endif()

# ── Global Compiler Options ─────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Dependencies (via vcpkg) ────────────────────────────────────────────────
find_package(OpenCV  4 REQUIRED COMPONENTS core imgproc imgcodecs)
find_package(Qt6     REQUIRED COMPONENTS Widgets Core Gui)

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
add_subdirectory(src/VisionCore)
add_subdirectory(src/VisionApp)

# ── Tests ────────────────────────────────────────────────────────────────────
if(GTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()

# ── Packaging (CPack) ────────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME "VisionStudio")
set(CPACK_PACKAGE_VENDOR "VisionStudio Team")
set(CPACK_PACKAGE_CONTACT "VisionStudio Team <visionstudio@users.noreply.github.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform OCR & Image Processing Studio")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt" COPYONLY)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")


if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    # NSIS premium installer configuration
    set(CPACK_NSIS_DISPLAY_NAME "VisionStudio")
    set(CPACK_NSIS_PACKAGE_NAME "VisionStudio")
    set(CPACK_NSIS_HELP_LINK "https://github.com/Hanam824/VisionStudio")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/Hanam824/VisionStudio")
    set(CPACK_NSIS_CONTACT "visionstudio@users.noreply.github.com")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
elseif(UNIX)
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
