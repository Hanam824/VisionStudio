#pragma once

#include "Export.h"

#include <opencv2/core.hpp>
#include <string>

namespace vision {

// ── ImageProcessor ──────────────────────────────────────────────────────────
/// Encapsulates all OpenCV-based image preprocessing operations.
/// This class is internal to VisionCore and is NOT exposed across the DLL
/// boundary — only IVisionEngine is public.
class ImageProcessor {
public:
    ImageProcessor()  = default;
    ~ImageProcessor() = default;

    // Non-copyable, movable.
    ImageProcessor(const ImageProcessor&)            = delete;
    ImageProcessor& operator=(const ImageProcessor&) = delete;
    ImageProcessor(ImageProcessor&&)                 = default;
    ImageProcessor& operator=(ImageProcessor&&)      = default;

    // ── Loading ─────────────────────────────────────────────────────────
    /// Load an image from disk into the internal cv::Mat.
    bool load(const std::string& filePath);

    /// Replace the current image with a provided cv::Mat (deep copy).
    void setImage(const cv::Mat& image);

    // ── Preprocessing Pipeline ──────────────────────────────────────────
    /// Convert to grayscale. Returns *this for chaining.
    ImageProcessor& toGrayscale();

    /// Apply adaptive thresholding.
    /// @param blockSize Odd number for the local neighborhood size.
    /// @param C         Constant subtracted from mean.
    ImageProcessor& applyThreshold(int blockSize = 11, double C = 2.0);

    /// Attempt automatic perspective correction (deskew).
    ImageProcessor& correctPerspective();

    /// Apply Gaussian blur for noise reduction.
    /// @param kernelSize Odd kernel size (e.g. 3, 5, 7).
    ImageProcessor& blur(int kernelSize = 3);

    // ── Access ──────────────────────────────────────────────────────────
    /// Returns the current (possibly processed) image.
    [[nodiscard]] const cv::Mat& image() const { return m_image; }

    /// Mutable access (for advanced operations).
    [[nodiscard]] cv::Mat& image() { return m_image; }

    /// Returns true if an image has been loaded.
    [[nodiscard]] bool hasImage() const { return !m_image.empty(); }

private:
    cv::Mat m_image;  ///< Current working image (BGR or grayscale).
};

} // namespace vision
