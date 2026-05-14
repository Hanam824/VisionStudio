#include "VisionCore/VisionEngine.h"

#include <opencv2/imgcodecs.hpp>
#include <sstream>

namespace vision {

// ── Construction / Destruction ──────────────────────────────────────────────

VisionEngine::VisionEngine() = default;

VisionEngine::~VisionEngine() {
    if (m_ready.load()) {
        shutdown();
    }
}

// ── Lifecycle ───────────────────────────────────────────────────────────────

bool VisionEngine::initialize() {
    std::lock_guard lock(m_mutex);

    log(LogLevel::Info, "VisionEngine initializing...");

    // TODO: Load ncnn/ONNX models here when available.
    // For now, only the image-processing pipeline is active.

    m_ready.store(true);
    log(LogLevel::Info, "VisionEngine ready (" + versionString() + ").");
    return true;
}

void VisionEngine::shutdown() {
    std::lock_guard lock(m_mutex);

    log(LogLevel::Info, "VisionEngine shutting down.");
    m_ready.store(false);
}

bool VisionEngine::isReady() const {
    return m_ready.load();
}

// ── Image I/O ───────────────────────────────────────────────────────────────

bool VisionEngine::loadImage(const std::string& filePath) {
    std::lock_guard lock(m_mutex);

    log(LogLevel::Info, "Loading image: " + filePath);

    if (!m_processor.load(filePath)) {
        log(LogLevel::Error, "Failed to load image: " + filePath);
        return false;
    }

    const auto& img = m_processor.image();
    std::ostringstream oss;
    oss << "Image loaded: " << img.cols << "x" << img.rows
        << " (" << img.channels() << " channels)";
    log(LogLevel::Info, oss.str());
    return true;
}

const uint8_t* VisionEngine::getImageData(
    int& width, int& height, int& channels) const {
    std::lock_guard lock(m_mutex);

    if (!m_processor.hasImage()) {
        width = height = channels = 0;
        return nullptr;
    }

    const auto& img = m_processor.image();
    width    = img.cols;
    height   = img.rows;
    channels = img.channels();
    return img.data;
}

// ── Preprocessing ───────────────────────────────────────────────────────────

bool VisionEngine::preprocess(const PreprocessOptions& opts) {
    std::lock_guard lock(m_mutex);

    if (!m_processor.hasImage()) {
        log(LogLevel::Warning, "No image loaded — cannot preprocess.");
        return false;
    }

    log(LogLevel::Info, "Preprocessing image...");

    if (opts.perspectiveCorrect) {
        log(LogLevel::Debug, "  Perspective correction...");
        m_processor.correctPerspective();
    }

    if (opts.grayscale) {
        log(LogLevel::Debug, "  Converting to grayscale...");
        m_processor.toGrayscale();
    }

    if (opts.threshold) {
        log(LogLevel::Debug, "  Applying adaptive threshold...");
        m_processor.applyThreshold(opts.thresholdBlockSize, opts.thresholdC);
    }

    log(LogLevel::Info, "Preprocessing complete.");
    return true;
}

// ── OCR / Inference ─────────────────────────────────────────────────────────

std::vector<OcrResult> VisionEngine::runOcr() {
    std::lock_guard lock(m_mutex);

    if (!m_processor.hasImage()) {
        log(LogLevel::Warning, "No image loaded — cannot run OCR.");
        return {};
    }

    log(LogLevel::Info, "Running OCR inference...");

    // TODO: Implement actual ncnn/ONNX inference pipeline.
    // For now, return an empty result set as a placeholder.
    log(LogLevel::Warning, "OCR inference not yet implemented — returning empty results.");

    return {};
}

// ── Logging ─────────────────────────────────────────────────────────────────

void VisionEngine::setLogCallback(LogCallback callback) {
    std::lock_guard lock(m_mutex);
    m_logCallback = std::move(callback);
}

void VisionEngine::log(LogLevel level, const std::string& msg) const {
    if (m_logCallback) {
        m_logCallback(level, msg);
    }
}

// ── Info ────────────────────────────────────────────────────────────────────

std::string VisionEngine::versionString() const {
    return "VisionCore 0.1.0";
}

} // namespace vision

// ── Factory Function (C-linkage) ────────────────────────────────────────────

VISIONCORE_C_API vision::IVisionEngine* createVisionEngine() {
    return new vision::VisionEngine();
}
