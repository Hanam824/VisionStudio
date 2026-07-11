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
/// Parses raw OCR results into structured ReceiptData.
VISIONCORE_API ReceiptData parseReceiptOcr(const std::vector<OcrResult>& ocrResults);

/// Formats ReceiptData as clean readable Markdown/Text table.
VISIONCORE_API std::string formatReceiptAsText(const ReceiptData& data);

/// Formats ReceiptData as JSON string.
VISIONCORE_API std::string formatReceiptAsJson(const ReceiptData& data);

} // namespace vision
