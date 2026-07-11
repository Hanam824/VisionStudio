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
/// Right dock panel for inspecting and modifying parsed receipt data and raw OCR results.
class ReceiptOcrPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit ReceiptOcrPanel(QWidget* parent = nullptr);
    ~ReceiptOcrPanel() override = default;

    /// Populate the panel from OCR results.
    void setOcrResults(const std::vector<vision::OcrResult>& results);

    /// Clear all data.
    void clearData();

    /// Highlight a specific OCR line index.
    void selectOcrLine(int index);

signals:
    /// Emitted when user selects an OCR line index in the list.
    void ocrLineSelected(int index);

    /// Emitted when the user edits, adds, or removes raw OCR lines.
    void ocrResultsModified(const std::vector<vision::OcrResult>& results);

    /// Emitted when data is copied to clipboard (for status bar updates).
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
