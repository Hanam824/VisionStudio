#include <gtest/gtest.h>

#include "VisionCore/ReceiptParser.h"
#include "VisionCore/VisionEngine.h"

using namespace vision;

TEST(ReceiptParserTest, ParseReceiptOcrBasicLayout) {
    std::vector<OcrResult> lines = {
        {"VISION CAFE", 0.98f, 10, 10, 100, 20},
        {"Date: 2026-07-11", 0.97f, 10, 40, 100, 20},
        {"Invoice #INV-88219", 0.96f, 10, 70, 100, 20},
        {"1x Espresso $3.50", 0.95f, 10, 100, 100, 20},
        {"2x Butter Croissant $7.00", 0.94f, 10, 130, 100, 20},
        {"Subtotal: $10.50", 0.96f, 10, 160, 100, 20},
        {"Tax: $0.84", 0.96f, 10, 190, 100, 20},
        {"Total: $11.34", 0.97f, 10, 220, 100, 20}
    };

    ReceiptData data = parseReceiptOcr(lines);

    EXPECT_EQ(data.merchant, "VISION CAFE");
    EXPECT_EQ(data.date, "2026-07-11");
    EXPECT_EQ(data.invoiceNumber, "INV-88219");
    ASSERT_EQ(data.items.size(), 2u);

    EXPECT_EQ(data.items[0].description, "Espresso");
    EXPECT_EQ(data.items[0].quantity, 1);
    EXPECT_DOUBLE_EQ(data.items[0].totalPrice, 3.50);

    EXPECT_EQ(data.items[1].description, "Butter Croissant");
    EXPECT_EQ(data.items[1].quantity, 2);
    EXPECT_DOUBLE_EQ(data.items[1].totalPrice, 7.00);
    EXPECT_DOUBLE_EQ(data.items[1].unitPrice, 3.50);

    EXPECT_DOUBLE_EQ(data.subtotal, 10.50);
    EXPECT_DOUBLE_EQ(data.tax, 0.84);
    EXPECT_DOUBLE_EQ(data.total, 11.34);
}

TEST(ReceiptParserTest, FormatTextAndJsonNotEmpty) {
    ReceiptData data;
    data.merchant = "TEST SHOP";
    data.date = "2026-07-11";
    data.items.push_back({"Item A", 1, 5.0, 5.0});
    data.subtotal = 5.0;
    data.total = 5.0;

    std::string text = formatReceiptAsText(data);
    std::string json = formatReceiptAsJson(data);

    EXPECT_FALSE(text.empty());
    EXPECT_NE(text.find("TEST SHOP"), std::string::npos);
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("\"merchant\": \"TEST SHOP\""), std::string::npos);
}

TEST(VisionEngineTest, RunOcrReturnsResults) {
    VisionEngine engine;
    EXPECT_TRUE(engine.initialize());

    // Run OCR on empty image should return empty
    auto emptyRes = engine.runOcr();
    EXPECT_TRUE(emptyRes.empty());
}

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

TEST(ReceiptParserTest, GenerateSampleReceiptImage) {
    cv::Mat receipt(650, 420, CV_8UC3, cv::Scalar(248, 248, 245));
    cv::rectangle(receipt, cv::Point(8, 8), cv::Point(412, 642), cv::Scalar(210, 210, 210), 2);

    auto drawLineText = [&](const std::string& txt, int y, double scale = 0.6, int thickness = 1) {
        cv::putText(receipt, txt, cv::Point(30, y), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(30, 30, 30), thickness, cv::LINE_AA);
    };

    drawLineText("VISION CAFE - COFFEE & BAKERY", 55, 0.65, 2);
    drawLineText("Date: 2026-07-11", 105);
    drawLineText("Invoice #INV-88219", 145);
    cv::line(receipt, cv::Point(25, 170), cv::Point(395, 170), cv::Scalar(150, 150, 150), 1);
    drawLineText("1x Espresso $3.50", 215);
    drawLineText("2x Butter Croissant $7.00", 265);
    drawLineText("1x Avocado Toast $8.50", 315);
    cv::line(receipt, cv::Point(25, 350), cv::Point(395, 350), cv::Scalar(150, 150, 150), 1);
    drawLineText("Subtotal: $19.00", 400);
    drawLineText("Tax: $1.52", 450);
    drawLineText("Total: $20.52", 505, 0.75, 2);

    bool saved = cv::imwrite("sample_receipt.png", receipt);
    EXPECT_TRUE(saved);
}

TEST(VisionEngineTest, EndToEndReceiptOcrAndParse) {
    VisionEngine engine;
    ASSERT_TRUE(engine.initialize());
    ASSERT_TRUE(engine.loadImage("sample_receipt.png"));

    auto results = engine.runOcr();
    EXPECT_FALSE(results.empty());

    ReceiptData parsed = parseReceiptOcr(results);
    EXPECT_EQ(parsed.merchant, "VISION CAFE - COFFEE & BAKERY");
    EXPECT_EQ(parsed.date, "2026-07-11");
    EXPECT_EQ(parsed.invoiceNumber, "INV-88219");
    EXPECT_GE(parsed.items.size(), 1u);
    EXPECT_GT(parsed.total, 0.0);
}
