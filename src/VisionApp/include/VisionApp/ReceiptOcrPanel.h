#pragma once

#include "VisionCore/IVisionEngine.h"
#include "VisionCore/ReceiptParser.h"

#include <QDockWidget>
#include <vector>

class QLineEdit;
class QDoubleSpinBox;
class QTableWidget;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTabWidget;

// ── ReceiptOcrPanel ─────────────────────────────────────────────────────────
/**
 * @brief Right dock panel for inspecting and modifying parsed receipt data and raw OCR results.
 */
class ReceiptOcrPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit ReceiptOcrPanel(QWidget* parent = nullptr);
    ~ReceiptOcrPanel() override = default;

    /**
     * @brief Populate the panel from OCR results.
     * @param results Detected OCR text regions; parsed into structured receipt data.
     */
    void setOcrResults(const std::vector<vision::OcrResult>& results);

    /**
     * @brief Clear all data, including form fields, items table, totals, and the raw OCR list.
     */
    void clearData();

    /**
     * @brief Highlight a specific OCR line index in the raw OCR list.
     * @param index Zero-based index into the raw OCR results. No-op if out of range.
     */
    void selectOcrLine(int index);

signals:
    /**
     * @brief Emitted when the user selects an OCR line index in the list.
     * @param index Zero-based index of the selected OCR line.
     */
    void ocrLineSelected(int index);

    /**
     * @brief Emitted when the user edits, adds, or removes raw OCR lines.
     * @param results The full, updated set of raw OCR results.
     */
    void ocrResultsModified(const std::vector<vision::OcrResult>& results);

    /**
     * @brief Emitted when data is copied to clipboard, for status bar updates.
     * @param msg Human-readable status message to display.
     */
    void statusMessageRequested(const QString& msg);

private slots:
    void onAddItem();
    void onRemoveItem();
    void onTableItemChanged();
    void onCopyTextReceipt();
    void onCopyJsonReceipt();
    void onCopyRawOcr();
    void onOcrListClicked(int row);
    void onAddOcrLine();
    void onRemoveOcrLine();
    void onOcrItemChanged(QListWidgetItem* item);

private:
    void setupUi();
    void updateTotalsFromTable();
    vision::ReceiptData collectCurrentReceiptData() const;

    /**
     * @brief Push parsed receipt data into the metadata fields, items table, and totals.
     * @param receiptData Structured receipt data to display.
     */
    void applyParsedReceipt(const vision::ReceiptData& receiptData);

    // ── Form Controls ───────────────────────────────────────────────────
    QLineEdit*      m_merchantEdit      = nullptr;
    QLineEdit*      m_dateEdit          = nullptr;
    QLineEdit*      m_invoiceEdit       = nullptr;

    QTableWidget*   m_itemsTable        = nullptr;
    QDoubleSpinBox* m_subtotalSpin      = nullptr;
    QDoubleSpinBox* m_taxSpin           = nullptr;
    QDoubleSpinBox* m_totalSpin         = nullptr;

    // ── Raw OCR Controls ────────────────────────────────────────────────
    QListWidget*    m_ocrListWidget     = nullptr;

    // ── Data ────────────────────────────────────────────────────────────
    std::vector<vision::OcrResult> m_rawOcrResults;
    bool                           m_updatingTable = false;
};
