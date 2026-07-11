#include "VisionCore/ReceiptParser.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <regex>
#include <sstream>

namespace vision {

static std::string trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static std::string toLower(const std::string& str) {
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

ReceiptData parseReceiptOcr(const std::vector<OcrResult>& ocrResults) {
    ReceiptData data;

    // Sort OCR results top-to-bottom, then left-to-right.
    std::vector<OcrResult> lines = ocrResults;
    std::sort(lines.begin(), lines.end(), [](const OcrResult& a, const OcrResult& b) {
        if (std::abs(a.y - b.y) > 10) {
            return a.y < b.y;
        }
        return a.x < b.x;
    });

    std::regex qtyItemPriceRegex(R"(^(?:(\d+)x\s+)?(.+?)\s+\$?(\d+\.\d{2})$)", std::regex::icase);
    std::regex dateRegex(R"((\d{4}[-/.]\d{2}[-/.]\d{2}|\d{2}[-/.]\d{2}[-/.]\d{4}))");
    std::regex labelValRegex(R"(^([^:]+):\s*(.*)$)");

    bool foundMerchant = false;

    for (const auto& r : lines) {
        std::string line = trimString(r.text);
        if (line.empty()) continue;

        std::string lower = toLower(line);

        // Check for totals
        if (lower.find("subtotal") != std::string::npos) {
            std::smatch match;
            std::regex priceReg(R"(\$?(\d+\.\d{2}))");
            if (std::regex_search(line, match, priceReg)) {
                data.subtotal = std::stod(match[1].str());
            }
            continue;
        }
        if (lower.find("tax") != std::string::npos) {
            std::smatch match;
            std::regex priceReg(R"(\$?(\d+\.\d{2}))");
            if (std::regex_search(line, match, priceReg)) {
                data.tax = std::stod(match[1].str());
            }
            continue;
        }
        if (lower.find("total") != std::string::npos && lower.find("subtotal") == std::string::npos) {
            std::smatch match;
            std::regex priceReg(R"(\$?(\d+\.\d{2}))");
            if (std::regex_search(line, match, priceReg)) {
                data.total = std::stod(match[1].str());
            }
            continue;
        }

        // Check for date
        std::smatch dMatch;
        if (std::regex_search(line, dMatch, dateRegex)) {
            if (data.date.empty()) {
                data.date = dMatch[1].str();
            }
            continue;
        }

        // Check for Invoice number (e.g. "Invoice #INV-88219" or "Invoice: INV-88219")
        if (lower.find("inv") != std::string::npos || lower.find("receipt #") != std::string::npos || lower.find("order #") != std::string::npos) {
            std::smatch invMatch;
            std::regex invReg(R"((?:Invoice|Inv|Receipt|Order)[\s#:]*([A-Za-z0-9_-]+))", std::regex_constants::icase);
            if (std::regex_search(line, invMatch, invReg)) {
                data.invoiceNumber = invMatch[1].str();
                continue;
            }
        }

        // Check for Date: or Invoice: prefix
        std::smatch lvMatch;
        if (std::regex_match(line, lvMatch, labelValRegex)) {
            std::string label = toLower(trimString(lvMatch[1].str()));
            std::string val = trimString(lvMatch[2].str());
            if (label.find("date") != std::string::npos) {
                data.date = val;
                continue;
            }
            if (label.find("inv") != std::string::npos || label.find("receipt") != std::string::npos || label.find("order") != std::string::npos) {
                data.invoiceNumber = val;
                continue;
            }
        }

        // Try line item pattern (e.g. "2x Croissant $7.00" or "Espresso $3.50")
        std::smatch iMatch;
        if (std::regex_match(line, iMatch, qtyItemPriceRegex)) {
            ReceiptItem item;
            std::string qtyStr = iMatch[1].str();
            item.quantity = qtyStr.empty() ? 1 : std::stoi(qtyStr);
            item.description = trimString(iMatch[2].str());
            item.totalPrice = std::stod(iMatch[3].str());
            item.unitPrice = (item.quantity > 0) ? (item.totalPrice / item.quantity) : item.totalPrice;
            data.items.push_back(item);
            continue;
        }

        // If merchant is not set yet and this line doesn't look like a number/item, use it as Merchant
        if (!foundMerchant && line.size() > 2) {
            data.merchant = line;
            foundMerchant = true;
        }
    }

    // Auto-calculate subtotal/total if missing
    if (data.subtotal == 0.0 && !data.items.empty()) {
        for (const auto& item : data.items) {
            data.subtotal += item.totalPrice;
        }
    }
    if (data.total == 0.0) {
        data.total = data.subtotal + data.tax;
    }

    return data;
}

std::string formatReceiptAsText(const ReceiptData& data) {
    std::ostringstream oss;
    oss << "=== RECEIPT / BILL ===\n";
    if (!data.merchant.empty())      oss << "Merchant: " << data.merchant << "\n";
    if (!data.date.empty())          oss << "Date:     " << data.date << "\n";
    if (!data.invoiceNumber.empty()) oss << "Invoice#: " << data.invoiceNumber << "\n";
    oss << "----------------------------------------\n";
    for (const auto& item : data.items) {
        oss << std::left << std::setw(24) << item.description
            << " " << std::setw(3) << item.quantity << "x "
            << "$" << std::fixed << std::setprecision(2) << item.totalPrice << "\n";
    }
    oss << "----------------------------------------\n";
    oss << std::left << std::setw(28) << "Subtotal:" << "$" << std::fixed << std::setprecision(2) << data.subtotal << "\n";
    oss << std::left << std::setw(28) << "Tax:"      << "$" << std::fixed << std::setprecision(2) << data.tax << "\n";
    oss << std::left << std::setw(28) << "TOTAL:"    << "$" << std::fixed << std::setprecision(2) << data.total << "\n";
    return oss.str();
}

std::string formatReceiptAsJson(const ReceiptData& data) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"merchant\": \"" << data.merchant << "\",\n";
    oss << "  \"date\": \"" << data.date << "\",\n";
    oss << "  \"invoiceNumber\": \"" << data.invoiceNumber << "\",\n";
    oss << "  \"items\": [\n";
    for (size_t i = 0; i < data.items.size(); ++i) {
        const auto& item = data.items[i];
        oss << "    {\n";
        oss << "      \"description\": \"" << item.description << "\",\n";
        oss << "      \"quantity\": " << item.quantity << ",\n";
        oss << "      \"unitPrice\": " << std::fixed << std::setprecision(2) << item.unitPrice << ",\n";
        oss << "      \"totalPrice\": " << std::fixed << std::setprecision(2) << item.totalPrice << "\n";
        oss << "    }" << (i + 1 < data.items.size() ? "," : "") << "\n";
    }
    oss << "  ],\n";
    oss << "  \"subtotal\": " << std::fixed << std::setprecision(2) << data.subtotal << ",\n";
    oss << "  \"tax\": " << std::fixed << std::setprecision(2) << data.tax << ",\n";
    oss << "  \"total\": " << std::fixed << std::setprecision(2) << data.total << "\n";
    oss << "}\n";
    return oss.str();
}

} // namespace vision
