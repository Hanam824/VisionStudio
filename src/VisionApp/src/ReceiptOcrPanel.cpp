#include "VisionApp/ReceiptOcrPanel.h"

#include <QApplication>
#include <QClipboard>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

ReceiptOcrPanel::ReceiptOcrPanel(QWidget* parent)
    : QDockWidget(tr("Receipt & OCR Inspector"), parent)
{
    setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    setMinimumWidth(380);

    setupUi();
}

void ReceiptOcrPanel::setupUi() {
    auto* container = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    auto* tabs = new QTabWidget(container);

    // ── Tab 1: Parsed Receipt Editor ────────────────────────────────────────
    auto* receiptTab = new QWidget(tabs);
    auto* receiptLayout = new QVBoxLayout(receiptTab);
    receiptLayout->setContentsMargins(6, 6, 6, 6);
    receiptLayout->setSpacing(8);

    // Header Metadata Group
    auto* metaGroup = new QGroupBox(tr("Receipt Metadata"), receiptTab);
    auto* formLayout = new QFormLayout(metaGroup);
    m_merchantEdit = new QLineEdit(metaGroup);
    m_merchantEdit->setPlaceholderText(tr("e.g. VISION CAFE"));
    m_dateEdit = new QLineEdit(metaGroup);
    m_dateEdit->setPlaceholderText(tr("YYYY-MM-DD"));
    m_invoiceEdit = new QLineEdit(metaGroup);
    m_invoiceEdit->setPlaceholderText(tr("e.g. INV-1004"));

    formLayout->addRow(tr("Merchant:"), m_merchantEdit);
    formLayout->addRow(tr("Date:"), m_dateEdit);
    formLayout->addRow(tr("Invoice #:"), m_invoiceEdit);
    receiptLayout->addWidget(metaGroup);

    // Items Table Group
    auto* itemsGroup = new QGroupBox(tr("Line Items (Editable)"), receiptTab);
    auto* itemsLayout = new QVBoxLayout(itemsGroup);

    m_itemsTable = new QTableWidget(0, 4, itemsGroup);
    m_itemsTable->setHorizontalHeaderLabels({tr("Description"), tr("Qty"), tr("Price ($)"), tr("Total ($)")});
    m_itemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_itemsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    itemsLayout->addWidget(m_itemsTable);

    auto* itemBtnsLayout = new QHBoxLayout();
    auto* addBtn = new QPushButton(tr("+ Add Item"), itemsGroup);
    auto* removeBtn = new QPushButton(tr("- Remove Item"), itemsGroup);
    itemBtnsLayout->addWidget(addBtn);
    itemBtnsLayout->addWidget(removeBtn);
    itemBtnsLayout->addStretch();
    itemsLayout->addLayout(itemBtnsLayout);

    receiptLayout->addWidget(itemsGroup);

    // Totals Group
    auto* totalsGroup = new QGroupBox(tr("Financial Totals"), receiptTab);
    auto* totalsLayout = new QFormLayout(totalsGroup);

    m_subtotalSpin = new QDoubleSpinBox(totalsGroup);
    m_subtotalSpin->setRange(0.0, 1000000.0);
    m_subtotalSpin->setPrefix("$ ");

    m_taxSpin = new QDoubleSpinBox(totalsGroup);
    m_taxSpin->setRange(0.0, 1000000.0);
    m_taxSpin->setPrefix("$ ");

    m_totalSpin = new QDoubleSpinBox(totalsGroup);
    m_totalSpin->setRange(0.0, 1000000.0);
    m_totalSpin->setPrefix("$ ");

    totalsLayout->addRow(tr("Subtotal:"), m_subtotalSpin);
    totalsLayout->addRow(tr("Tax:"), m_taxSpin);
    totalsLayout->addRow(tr("Total:"), m_totalSpin);
    receiptLayout->addWidget(totalsGroup);

    tabs->addTab(receiptTab, tr("Parsed Receipt"));

    // ── Tab 2: Raw OCR Detected Text Lines ──────────────────────────────────
    auto* ocrTab = new QWidget(tabs);
    auto* ocrLayout = new QVBoxLayout(ocrTab);
    ocrLayout->setContentsMargins(6, 6, 6, 6);

    auto* ocrLabel = new QLabel(tr("Double-click any line to correct scanner mistakes:"), ocrTab);
    ocrLayout->addWidget(ocrLabel);

    m_ocrListWidget = new QListWidget(ocrTab);
    ocrLayout->addWidget(m_ocrListWidget);

    auto* ocrBtnsLayout = new QHBoxLayout();
    auto* addOcrBtn = new QPushButton(tr("+ Add OCR Line"), ocrTab);
    auto* removeOcrBtn = new QPushButton(tr("- Remove OCR Line"), ocrTab);
    ocrBtnsLayout->addWidget(addOcrBtn);
    ocrBtnsLayout->addWidget(removeOcrBtn);
    ocrBtnsLayout->addStretch();
    ocrLayout->addLayout(ocrBtnsLayout);

    tabs->addTab(ocrTab, tr("Raw OCR Lines"));

    mainLayout->addWidget(tabs);

    // ── Action Buttons (Copy Buttons) ───────────────────────────────────────
    auto* copyLayout = new QVBoxLayout();
    auto* copyRow1 = new QHBoxLayout();
    auto* copyTextBtn = new QPushButton(tr("📋 Copy Formatted Receipt"), container);
    auto* copyJsonBtn = new QPushButton(tr("📋 Copy Receipt JSON"), container);
    copyRow1->addWidget(copyTextBtn);
    copyRow1->addWidget(copyJsonBtn);

    auto* copyOcrBtn = new QPushButton(tr("📋 Copy All OCR Text"), container);
    copyLayout->addLayout(copyRow1);
    copyLayout->addWidget(copyOcrBtn);

    mainLayout->addLayout(copyLayout);

    setWidget(container);

    // Signals
    connect(addBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onAddItem);
    connect(removeBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onRemoveItem);
    connect(m_itemsTable, &QTableWidget::itemChanged, this, &ReceiptOcrPanel::onTableItemChanged);

    connect(copyTextBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onCopyTextReceipt);
    connect(copyJsonBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onCopyJsonReceipt);
    connect(copyOcrBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onCopyRawOcr);

    connect(m_ocrListWidget, &QListWidget::currentRowChanged, this, &ReceiptOcrPanel::onOcrListClicked);
    connect(addOcrBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onAddOcrLine);
    connect(removeOcrBtn, &QPushButton::clicked, this, &ReceiptOcrPanel::onRemoveOcrLine);
    connect(m_ocrListWidget, &QListWidget::itemChanged, this, &ReceiptOcrPanel::onOcrItemChanged);
}

void ReceiptOcrPanel::clearData() {
    m_updatingTable = true;
    m_merchantEdit->clear();
    m_dateEdit->clear();
    m_invoiceEdit->clear();
    m_itemsTable->setRowCount(0);
    m_subtotalSpin->setValue(0.0);
    m_taxSpin->setValue(0.0);
    m_totalSpin->setValue(0.0);
    m_ocrListWidget->clear();
    m_rawOcrResults.clear();
    m_updatingTable = false;
}

void ReceiptOcrPanel::setOcrResults(const std::vector<vision::OcrResult>& results) {
    m_rawOcrResults = results;
    m_updatingTable = true;

    // Populate Raw OCR List
    m_ocrListWidget->clear();
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        auto* item = new QListWidgetItem(QString::fromStdString(r.text), m_ocrListWidget);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setToolTip(QString("Line %1 | Confidence: %2%").arg(i + 1).arg(static_cast<int>(r.confidence * 100)));
    }

    // Parse into structured receipt
    vision::ReceiptData receiptData = vision::parseReceiptOcr(results);
    applyParsedReceipt(receiptData);

    m_updatingTable = false;
}

void ReceiptOcrPanel::applyParsedReceipt(const vision::ReceiptData& receiptData) {
    m_merchantEdit->setText(QString::fromStdString(receiptData.merchant));
    m_dateEdit->setText(QString::fromStdString(receiptData.date));
    m_invoiceEdit->setText(QString::fromStdString(receiptData.invoiceNumber));

    m_itemsTable->setRowCount(static_cast<int>(receiptData.items.size()));
    for (size_t i = 0; i < receiptData.items.size(); ++i) {
        const auto& item = receiptData.items[i];
        m_itemsTable->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::fromStdString(item.description)));
        m_itemsTable->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::number(item.quantity)));
        m_itemsTable->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::number(item.unitPrice, 'f', 2)));
        m_itemsTable->setItem(static_cast<int>(i), 3, new QTableWidgetItem(QString::number(item.totalPrice, 'f', 2)));
    }

    m_subtotalSpin->setValue(receiptData.subtotal);
    m_taxSpin->setValue(receiptData.tax);
    m_totalSpin->setValue(receiptData.total);
}

void ReceiptOcrPanel::selectOcrLine(int index) {
    if (index >= 0 && index < m_ocrListWidget->count()) {
        m_ocrListWidget->setCurrentRow(index);
    }
}

void ReceiptOcrPanel::onAddItem() {
    m_updatingTable = true;
    int row = m_itemsTable->rowCount();
    m_itemsTable->insertRow(row);
    m_itemsTable->setItem(row, 0, new QTableWidgetItem("New Item"));
    m_itemsTable->setItem(row, 1, new QTableWidgetItem("1"));
    m_itemsTable->setItem(row, 2, new QTableWidgetItem("0.00"));
    m_itemsTable->setItem(row, 3, new QTableWidgetItem("0.00"));
    m_updatingTable = false;
    updateTotalsFromTable();
}

void ReceiptOcrPanel::onRemoveItem() {
    int row = m_itemsTable->currentRow();
    if (row >= 0) {
        m_itemsTable->removeRow(row);
        updateTotalsFromTable();
    }
}

void ReceiptOcrPanel::onTableItemChanged() {
    if (m_updatingTable) return;
    updateTotalsFromTable();
}

void ReceiptOcrPanel::updateTotalsFromTable() {
    m_updatingTable = true;
    double sub = 0.0;
    for (int r = 0; r < m_itemsTable->rowCount(); ++r) {
        auto* qItem = m_itemsTable->item(r, 1);
        auto* pItem = m_itemsTable->item(r, 2);
        auto* tItem = m_itemsTable->item(r, 3);

        int qty = qItem ? qItem->text().toInt() : 1;
        double price = pItem ? pItem->text().toDouble() : 0.0;
        double lineTotal = qty * price;

        if (tItem) {
            tItem->setText(QString::number(lineTotal, 'f', 2));
        }
        sub += lineTotal;
    }
    m_subtotalSpin->setValue(sub);
    double currentTax = m_taxSpin->value();
    m_totalSpin->setValue(sub + currentTax);
    m_updatingTable = false;
}

vision::ReceiptData ReceiptOcrPanel::collectCurrentReceiptData() const {
    vision::ReceiptData receiptData;
    receiptData.merchant = m_merchantEdit->text().toStdString();
    receiptData.date = m_dateEdit->text().toStdString();
    receiptData.invoiceNumber = m_invoiceEdit->text().toStdString();

    for (int r = 0; r < m_itemsTable->rowCount(); ++r) {
        vision::ReceiptItem item;
        auto* dItem = m_itemsTable->item(r, 0);
        auto* qItem = m_itemsTable->item(r, 1);
        auto* pItem = m_itemsTable->item(r, 2);
        auto* tItem = m_itemsTable->item(r, 3);

        item.description = dItem ? dItem->text().toStdString() : "";
        item.quantity = qItem ? qItem->text().toInt() : 1;
        item.unitPrice = pItem ? pItem->text().toDouble() : 0.0;
        item.totalPrice = tItem ? tItem->text().toDouble() : (item.quantity * item.unitPrice);
        receiptData.items.push_back(item);
    }

    receiptData.subtotal = m_subtotalSpin->value();
    receiptData.tax = m_taxSpin->value();
    receiptData.total = m_totalSpin->value();
    return receiptData;
}

void ReceiptOcrPanel::onCopyTextReceipt() {
    vision::ReceiptData receiptData = collectCurrentReceiptData();
    std::string text = vision::formatReceiptAsText(receiptData);
    QApplication::clipboard()->setText(QString::fromStdString(text));
    emit statusMessageRequested(tr("Formatted receipt copied to clipboard."));
}

void ReceiptOcrPanel::onCopyJsonReceipt() {
    vision::ReceiptData receiptData = collectCurrentReceiptData();
    std::string json = vision::formatReceiptAsJson(receiptData);
    QApplication::clipboard()->setText(QString::fromStdString(json));
    emit statusMessageRequested(tr("Receipt JSON copied to clipboard."));
}

void ReceiptOcrPanel::onCopyRawOcr() {
    QStringList lines;
    for (const auto& r : m_rawOcrResults) {
        lines << QString::fromStdString(r.text);
    }
    QApplication::clipboard()->setText(lines.join("\n"));
    emit statusMessageRequested(tr("Raw OCR text copied to clipboard."));
}

void ReceiptOcrPanel::onOcrListClicked(int row) {
    if (row >= 0) {
        emit ocrLineSelected(row);
    }
}

void ReceiptOcrPanel::onAddOcrLine() {
    vision::OcrResult newRes;
    newRes.text = "New OCR Line $0.00";
    newRes.confidence = 1.0f;
    newRes.x = 20; newRes.y = 100; newRes.w = 150; newRes.h = 25;
    m_rawOcrResults.push_back(newRes);
    setOcrResults(m_rawOcrResults);
    emit ocrResultsModified(m_rawOcrResults);
}

void ReceiptOcrPanel::onRemoveOcrLine() {
    int row = m_ocrListWidget->currentRow();
    if (row >= 0 && row < static_cast<int>(m_rawOcrResults.size())) {
        m_rawOcrResults.erase(m_rawOcrResults.begin() + row);
        setOcrResults(m_rawOcrResults);
        emit ocrResultsModified(m_rawOcrResults);
    }
}

void ReceiptOcrPanel::onOcrItemChanged(QListWidgetItem* item) {
    if (m_updatingTable || !item) return;
    int row = m_ocrListWidget->row(item);
    if (row >= 0 && row < static_cast<int>(m_rawOcrResults.size())) {
        m_rawOcrResults[row].text = item->text().toStdString();

        // Re-parse structured receipt from modified OCR lines
        m_updatingTable = true;
        vision::ReceiptData receiptData = vision::parseReceiptOcr(m_rawOcrResults);
        applyParsedReceipt(receiptData);
        m_updatingTable = false;

        emit ocrResultsModified(m_rawOcrResults);
    }
}
