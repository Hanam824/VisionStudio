#pragma once

#include "IVisionEngine.h"

namespace vision {

// ── VisionEngine ────────────────────────────────────────────────────────────
/**
 * @brief Concrete implementation of IVisionEngine.
 *
 * Delegates image work to ImageProcessor and inference to ncnn/ONNX.
 * Uses the Pimpl idiom to keep internal implementation details private
 * across the DLL boundary.
 */
class VISIONCORE_API VisionEngine final : public IVisionEngine {
public:
    VisionEngine();
    ~VisionEngine() override;

    // Non-copyable, non-movable (singleton-like per instance).
    VisionEngine(const VisionEngine&)            = delete;
    VisionEngine& operator=(const VisionEngine&) = delete;

    // ── IVisionEngine overrides ─────────────────────────────────────────
    bool initialize() override;
    void shutdown() override;
    [[nodiscard]] bool isReady() const override;

    bool loadImage(const std::string& filePath) override;
    [[nodiscard]] ImageData getImageData() const override;

    bool preprocess(const PreprocessOptions& opts) override;
    std::vector<OcrResult> runOcr() override;

    void setLogCallback(LogCallback callback) override;
    [[nodiscard]] std::string versionString() const override;

private:
    void log(LogLevel level, const std::string& msg) const;

    struct Impl;
    Impl* m_impl;
};

} // namespace vision
