#include <gtest/gtest.h>

#include "VisionCore/ImageProcessor.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace vision;

// ── Helper: Create a test image ─────────────────────────────────────────────

static cv::Mat createTestImage(int width = 200, int height = 150) {
    cv::Mat img(height, width, CV_8UC3, cv::Scalar(128, 64, 32));
    // Draw some shapes for contour detection tests.
    cv::rectangle(img, cv::Point(20, 20), cv::Point(180, 130),
                  cv::Scalar(255, 255, 255), 2);
    cv::putText(img, "Test", cv::Point(60, 80),
                cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    return img;
}

// ── Tests ───────────────────────────────────────────────────────────────────

TEST(ImageProcessorTest, DefaultStateIsEmpty) {
    ImageProcessor proc;
    EXPECT_FALSE(proc.hasImage());
}

TEST(ImageProcessorTest, SetImageWorks) {
    ImageProcessor proc;
    cv::Mat img = createTestImage();
    proc.setImage(img);

    EXPECT_TRUE(proc.hasImage());
    EXPECT_EQ(proc.image().cols, 200);
    EXPECT_EQ(proc.image().rows, 150);
    EXPECT_EQ(proc.image().channels(), 3);
}

TEST(ImageProcessorTest, ToGrayscaleReducesChannels) {
    ImageProcessor proc;
    proc.setImage(createTestImage());
    proc.toGrayscale();

    EXPECT_TRUE(proc.hasImage());
    EXPECT_EQ(proc.image().channels(), 1);
}

TEST(ImageProcessorTest, ThresholdProducesBinary) {
    ImageProcessor proc;
    proc.setImage(createTestImage());
    proc.toGrayscale();
    proc.applyThreshold(11, 2.0);

    EXPECT_TRUE(proc.hasImage());
    EXPECT_EQ(proc.image().channels(), 1);

    // Check that values are binary (0 or 255).
    double minVal, maxVal;
    cv::minMaxLoc(proc.image(), &minVal, &maxVal);
    EXPECT_TRUE(minVal == 0 || minVal == 255);
    EXPECT_TRUE(maxVal == 0 || maxVal == 255);
}

TEST(ImageProcessorTest, BlurSmooths) {
    ImageProcessor proc;
    cv::Mat sharp = createTestImage();
    proc.setImage(sharp);

    // Compute Laplacian variance before blur (measure of sharpness).
    cv::Mat grayBefore;
    cv::cvtColor(proc.image(), grayBefore, cv::COLOR_BGR2GRAY);
    cv::Mat lap1;
    cv::Laplacian(grayBefore, lap1, CV_64F);
    cv::Scalar mu1, sigma1;
    cv::meanStdDev(lap1, mu1, sigma1);
    double varianceBefore = sigma1[0] * sigma1[0];

    // Apply blur.
    proc.blur(7);

    cv::Mat grayAfter;
    if (proc.image().channels() > 1) {
        cv::cvtColor(proc.image(), grayAfter, cv::COLOR_BGR2GRAY);
    } else {
        grayAfter = proc.image();
    }
    cv::Mat lap2;
    cv::Laplacian(grayAfter, lap2, CV_64F);
    cv::Scalar mu2, sigma2;
    cv::meanStdDev(lap2, mu2, sigma2);
    double varianceAfter = sigma2[0] * sigma2[0];

    // Blurred image should be less sharp.
    EXPECT_LT(varianceAfter, varianceBefore);
}

TEST(ImageProcessorTest, ChainingWorks) {
    ImageProcessor proc;
    proc.setImage(createTestImage());

    // Chaining calls should compile and run without errors.
    proc.toGrayscale().blur(3).applyThreshold();

    EXPECT_TRUE(proc.hasImage());
    EXPECT_EQ(proc.image().channels(), 1);
}

TEST(ImageProcessorTest, EmptyImageOperationsAreSafe) {
    ImageProcessor proc;

    // None of these should crash on an empty image.
    proc.toGrayscale();
    proc.applyThreshold();
    proc.blur();
    proc.correctPerspective();

    EXPECT_FALSE(proc.hasImage());
}
