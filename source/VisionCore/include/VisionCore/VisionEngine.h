#pragma once

#include "IVisionEngine.h"
#include "ImageProcessor.h"

#include <atomic>
#include <mutex>

namespace vision {

// ── VisionEngine ────────────────────────────────────────────────────────────
/// Concrete implementation of IVisionEngine.
/// Delegates image work to ImageProcessor and inference to ncnn/ONNX.
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
    [[nodiscard]] const uint8_t* getImageData(
        int& width, int& height, int& channels) const override;

    bool preprocess(const PreprocessOptions& opts) override;
    std::vector<OcrResult> runOcr() override;

    void setLogCallback(LogCallback callback) override;
    [[nodiscard]] std::string versionString() const override;

private:
    void log(LogLevel level, const std::string& msg) const;

    ImageProcessor   m_processor;       ///< Image preprocessing pipeline.
    LogCallback      m_logCallback;     ///< UI log sink.
    std::atomic_bool m_ready{false};    ///< Engine state flag.
    mutable std::mutex m_mutex;         ///< Thread safety for shared state.
};

} // namespace vision
