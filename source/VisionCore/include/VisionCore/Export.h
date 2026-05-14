#pragma once

// ── DLL Export / Import Macros ──────────────────────────────────────────────
// Cross-platform visibility control for VisionCore shared library.
// On Windows (MSVC): uses __declspec(dllexport/dllimport).
// On macOS/Linux (GCC/Clang): uses __attribute__((visibility("default"))).
//
// When building VisionCore, define VISIONCORE_EXPORTS.
// When consuming VisionCore (e.g. VisionApp), leave it undefined.

#if defined(_WIN32) || defined(_WIN64)
    #ifdef VISIONCORE_EXPORTS
        #define VISIONCORE_API __declspec(dllexport)
    #else
        #define VISIONCORE_API __declspec(dllimport)
    #endif
#elif defined(__APPLE__) || defined(__linux__)
    #ifdef VISIONCORE_EXPORTS
        #define VISIONCORE_API __attribute__((visibility("default")))
    #else
        #define VISIONCORE_API
    #endif
#else
    #define VISIONCORE_API
#endif

// Convenience macro for C-linkage factory functions (used for dynamic loading).
#define VISIONCORE_C_API extern "C" VISIONCORE_API
