#pragma once

#include "Export.h"
#include "IVisionEngine.h"

#include <string>
#include <vector>

namespace vision {

// ── Receipt Line Item ───────────────────────────────────────────────────────
struct ReceiptItem {
    std::string description;
    int         quantity    = 1;
    double      unitPrice   = 0.0;
    double      totalPrice  = 0.0;
};

// ── Structured Receipt Data ─────────────────────────────────────────────────
struct ReceiptData {
    std::string              merchant;
    std::string              date;
    std::string              invoiceNumber;
    std::vector<ReceiptItem> items;
    double                   subtotal = 0.0;
    double                   tax      = 0.0;
    double                   total    = 0.0;
};

// ── Receipt Parser Function ─────────────────────────────────────────────────
/**
 * @brief Parses raw OCR results into structured ReceiptData.
 * @param ocrResults Detected OCR text regions, in any order.
 * @return Structured receipt data (merchant, date, items, totals).
 */
VISIONCORE_API ReceiptData parseReceiptOcr(const std::vector<OcrResult>& ocrResults);

/**
 * @brief Formats ReceiptData as a clean, readable Markdown/Text table.
 * @param data Structured receipt data to format.
 * @return Formatted text block.
 */
VISIONCORE_API std::string formatReceiptAsText(const ReceiptData& data);

/**
 * @brief Formats ReceiptData as a JSON string.
 * @param data Structured receipt data to format.
 * @return JSON-formatted string.
 */
VISIONCORE_API std::string formatReceiptAsJson(const ReceiptData& data);

} // namespace vision
