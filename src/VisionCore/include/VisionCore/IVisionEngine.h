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
/**
 * @brief Signature: void(LogLevel level, const std::string& message)
 */
using LogCallback = std::function<void(LogLevel, const std::string&)>;

// ── OCR Result ──────────────────────────────────────────────────────────────
/**
 * @brief Holds a single recognized text region.
 */
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
/**
 * @brief Pure virtual interface for all vision/OCR operations.
 *
 * VisionApp interacts ONLY through this interface (LGPLv3 compliance).
 */
class VISIONCORE_API IVisionEngine {
public:
    virtual ~IVisionEngine() = default;

    // ── Lifecycle ───────────────────────────────────────────────────────
    /**
     * @brief Initialize the engine.
     * @return true on success.
     */
    virtual bool initialize() = 0;

    /**
     * @brief Shut down and release resources.
     */
    virtual void shutdown() = 0;

    /**
     * @brief Returns true if the engine is ready for work.
     */
    [[nodiscard]] virtual bool isReady() const = 0;

    // ── Image I/O ───────────────────────────────────────────────────────
    /**
     * @brief Load an image from disk.
     * @param filePath Path to the image file.
     * @return true on success.
     */
    virtual bool loadImage(const std::string& filePath) = 0;

    /**
     * @brief Get a deep copy of the currently loaded (or processed) image data.
     * @return Image data with buffer in BGR format (OpenCV default).
     */
    [[nodiscard]] virtual ImageData getImageData() const = 0;

    // ── Preprocessing ───────────────────────────────────────────────────
    /**
     * @brief Apply preprocessing to the loaded image.
     * @param opts Preprocessing options to apply.
     * @return true on success.
     */
    virtual bool preprocess(const PreprocessOptions& opts = {}) = 0;

    // ── OCR / Inference ─────────────────────────────────────────────────
    /**
     * @brief Run OCR on the (preprocessed) image.
     * @return Vector of recognized text regions.
     */
    virtual std::vector<OcrResult> runOcr() = 0;

    // ── Logging ─────────────────────────────────────────────────────────
    /**
     * @brief Register a callback to receive log messages from the engine.
     * @param callback Log sink invoked with severity level and message.
     */
    virtual void setLogCallback(LogCallback callback) = 0;

    // ── Info ────────────────────────────────────────────────────────────
    /**
     * @brief Engine name / version string.
     */
    [[nodiscard]] virtual std::string versionString() const = 0;
};

} // namespace vision

// ── Factory function (C-linkage for dynamic loading) ────────────────────────
using CreateEngineFunc = vision::IVisionEngine* (*)();

/**
 * @brief Creates a new IVisionEngine instance. Caller owns the returned pointer.
 *
 * Use with QLibrary / dlopen:
 * @code
 *   auto create = (CreateEngineFunc)lib.resolve("createVisionEngine");
 *   std::unique_ptr<IVisionEngine> engine(create());
 * @endcode
 * @return Newly allocated IVisionEngine instance.
 */
VISIONCORE_C_API vision::IVisionEngine* createVisionEngine();
