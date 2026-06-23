#pragma once

#include "Export.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace vision {

// ── Log Severity ────────────────────────────────────────────────────────────
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

// ── Log Callback ────────────────────────────────────────────────────────────
/// Signature: void(LogLevel level, const std::string& message)
using LogCallback = std::function<void(LogLevel, const std::string&)>;

// ── OCR Result ──────────────────────────────────────────────────────────────
/// Holds a single recognized text region.
struct OcrResult {
    std::string text;          ///< Recognized text content.
    float       confidence;    ///< Confidence score [0.0, 1.0].
    int         x, y, w, h;   ///< Bounding box in image coordinates.
};

// ── Preprocessing Options ───────────────────────────────────────────────────
struct PreprocessOptions {
    bool  grayscale          = true;   ///< Convert to grayscale first.
    bool  threshold          = true;   ///< Apply adaptive thresholding.
    int   thresholdBlockSize = 11;     ///< Block size for adaptive threshold.
    double thresholdC        = 2.0;    ///< Constant subtracted from mean.
    bool  perspectiveCorrect = false;  ///< Attempt perspective correction.
};

// ── ImageData ───────────────────────────────────────────────────────────────
struct ImageData {
    std::vector<uint8_t> buffer;
    int width = 0;
    int height = 0;
    int channels = 0;
    int step = 0;
};

// ── IVisionEngine (Abstract Interface) ──────────────────────────────────────
/// Pure virtual interface for all vision/OCR operations.
/// VisionApp interacts ONLY through this interface (LGPLv3 compliance).
class VISIONCORE_API IVisionEngine {
public:
    virtual ~IVisionEngine() = default;

    // ── Lifecycle ───────────────────────────────────────────────────────
    /// Initialize the engine. Returns true on success.
    virtual bool initialize() = 0;

    /// Shut down and release resources.
    virtual void shutdown() = 0;

    /// Returns true if the engine is ready for work.
    [[nodiscard]] virtual bool isReady() const = 0;

    // ── Image I/O ───────────────────────────────────────────────────────
    /// Load an image from disk. Returns true on success.
    virtual bool loadImage(const std::string& filePath) = 0;

    /// Get a deep copy of the currently loaded (or processed) image data.
    /// The returned buffer is in BGR format (OpenCV default).
    [[nodiscard]] virtual ImageData getImageData() const = 0;

    // ── Preprocessing ───────────────────────────────────────────────────
    /// Apply preprocessing to the loaded image.
    virtual bool preprocess(const PreprocessOptions& opts = {}) = 0;

    // ── OCR / Inference ─────────────────────────────────────────────────
    /// Run OCR on the (preprocessed) image.
    /// @return Vector of recognized text regions.
    virtual std::vector<OcrResult> runOcr() = 0;

    // ── Logging ─────────────────────────────────────────────────────────
    /// Register a callback to receive log messages from the engine.
    virtual void setLogCallback(LogCallback callback) = 0;

    // ── Info ────────────────────────────────────────────────────────────
    /// Engine name / version string.
    [[nodiscard]] virtual std::string versionString() const = 0;
};

} // namespace vision

// ── Factory function (C-linkage for dynamic loading) ────────────────────────
/// Creates a new IVisionEngine instance.  Caller owns the returned pointer.
/// Use with QLibrary / dlopen:
///   auto create = (CreateEngineFunc)lib.resolve("createVisionEngine");
///   std::unique_ptr<IVisionEngine> engine(create());
using CreateEngineFunc = vision::IVisionEngine* (*)();
VISIONCORE_C_API vision::IVisionEngine* createVisionEngine();
