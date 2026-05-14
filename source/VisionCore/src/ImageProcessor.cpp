#include "VisionCore/ImageProcessor.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace vision {

// ── Loading ─────────────────────────────────────────────────────────────────

bool ImageProcessor::load(const std::string& filePath) {
    m_image = cv::imread(filePath, cv::IMREAD_COLOR);
    return !m_image.empty();
}

void ImageProcessor::setImage(const cv::Mat& image) {
    m_image = image.clone();
}

// ── Preprocessing Pipeline ──────────────────────────────────────────────────

ImageProcessor& ImageProcessor::toGrayscale() {
    if (m_image.empty()) return *this;

    if (m_image.channels() == 3 || m_image.channels() == 4) {
        cv::cvtColor(m_image, m_image, cv::COLOR_BGR2GRAY);
    }
    return *this;
}

ImageProcessor& ImageProcessor::applyThreshold(int blockSize, double C) {
    if (m_image.empty()) return *this;

    // Threshold works on single-channel images.
    cv::Mat gray;
    if (m_image.channels() > 1) {
        cv::cvtColor(m_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = m_image;
    }

    // Ensure blockSize is odd and >= 3.
    blockSize = std::max(3, blockSize | 1);

    cv::adaptiveThreshold(
        gray, m_image,
        255,
        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
        cv::THRESH_BINARY,
        blockSize,
        C
    );
    return *this;
}

ImageProcessor& ImageProcessor::correctPerspective() {
    if (m_image.empty()) return *this;

    // Convert to grayscale for contour detection if needed.
    cv::Mat gray;
    if (m_image.channels() > 1) {
        cv::cvtColor(m_image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = m_image.clone();
    }

    // Detect edges.
    cv::Mat edges;
    cv::Canny(gray, edges, 50, 200);

    // Find contours.
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // Sort by area descending and look for a quadrilateral.
    std::sort(contours.begin(), contours.end(),
        [](const auto& a, const auto& b) {
            return cv::contourArea(a) > cv::contourArea(b);
        }
    );

    for (const auto& contour : contours) {
        double peri = cv::arcLength(contour, true);
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contour, approx, 0.02 * peri, true);

        if (approx.size() == 4 &&
            cv::contourArea(approx) > m_image.total() * 0.1) {
            // Order points: top-left, top-right, bottom-right, bottom-left.
            cv::Point2f src[4];
            // Sort by y first, then by x within each pair.
            std::sort(approx.begin(), approx.end(),
                [](const cv::Point& a, const cv::Point& b) {
                    return a.y < b.y;
                }
            );
            // Top two points.
            if (approx[0].x < approx[1].x) {
                src[0] = approx[0]; src[1] = approx[1];
            } else {
                src[0] = approx[1]; src[1] = approx[0];
            }
            // Bottom two points.
            if (approx[2].x < approx[3].x) {
                src[3] = approx[2]; src[2] = approx[3];
            } else {
                src[3] = approx[3]; src[2] = approx[2];
            }

            // Compute destination rectangle.
            float w1 = static_cast<float>(
                std::sqrt(std::pow(src[1].x - src[0].x, 2) +
                          std::pow(src[1].y - src[0].y, 2)));
            float w2 = static_cast<float>(
                std::sqrt(std::pow(src[2].x - src[3].x, 2) +
                          std::pow(src[2].y - src[3].y, 2)));
            float h1 = static_cast<float>(
                std::sqrt(std::pow(src[3].x - src[0].x, 2) +
                          std::pow(src[3].y - src[0].y, 2)));
            float h2 = static_cast<float>(
                std::sqrt(std::pow(src[2].x - src[1].x, 2) +
                          std::pow(src[2].y - src[1].y, 2)));

            float maxW = std::max(w1, w2);
            float maxH = std::max(h1, h2);

            cv::Point2f dst[4] = {
                {0,    0},
                {maxW, 0},
                {maxW, maxH},
                {0,    maxH}
            };

            cv::Mat M = cv::getPerspectiveTransform(src, dst);
            cv::warpPerspective(m_image, m_image, M,
                cv::Size(static_cast<int>(maxW), static_cast<int>(maxH)));
            break;
        }
    }
    return *this;
}

ImageProcessor& ImageProcessor::blur(int kernelSize) {
    if (m_image.empty()) return *this;

    kernelSize = std::max(1, kernelSize | 1);
    cv::GaussianBlur(m_image, m_image, cv::Size(kernelSize, kernelSize), 0);
    return *this;
}

} // namespace vision
