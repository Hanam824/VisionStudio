#include "VisionApp/LogConsole.h"

#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QScrollBar>

// ── Construction ────────────────────────────────────────────────────────────

LogConsole::LogConsole(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    // Text display.
    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setMaximumBlockCount(5000);  // Prevent unbounded growth.
    m_textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    // Monospace font.
    QFont monoFont("Consolas", 9);
    monoFont.setStyleHint(QFont::Monospace);
    m_textEdit->setFont(monoFont);

    // Dark console styling.
    m_textEdit->setStyleSheet(
        "QPlainTextEdit {"
        "  background-color: #1a1a2e;"
        "  color: #e0e0e0;"
        "  border: none;"
        "  selection-background-color: #3a3a5c;"
        "}"
    );

    layout->addWidget(m_textEdit);

    // Bottom toolbar.
    auto* bottomBar = new QHBoxLayout();
    bottomBar->setContentsMargins(4, 0, 4, 2);
    bottomBar->addStretch();

    auto* clearBtn = new QPushButton(tr("Clear"), this);
    clearBtn->setFixedHeight(22);
    clearBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #2a2a4a;"
        "  color: #c0c0c0;"
        "  border: 1px solid #3a3a5c;"
        "  border-radius: 3px;"
        "  padding: 0 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3a3a5c;"
        "}"
    );
    connect(clearBtn, &QPushButton::clicked, this, &LogConsole::onClear);
    bottomBar->addWidget(clearBtn);

    layout->addLayout(bottomBar);

    setLayout(layout);
}

// ── Logging ─────────────────────────────────────────────────────────────────

void LogConsole::appendMessage(int level, const QString& message) {
    // Colour by level.
    QString color;
    QString prefix;
    switch (level) {
        case 0:  color = "#808080"; prefix = "DBG"; break;  // Debug  → gray
        case 1:  color = "#e0e0e0"; prefix = "INF"; break;  // Info   → white
        case 2:  color = "#f0c040"; prefix = "WRN"; break;  // Warning→ yellow
        case 3:  color = "#f04040"; prefix = "ERR"; break;  // Error  → red
        default: color = "#e0e0e0"; prefix = "???"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");

    QString html = QString(
        "<span style=\"color:#606080\">%1</span> "
        "<span style=\"color:%2\">[%3] %4</span>")
        .arg(timestamp, color, prefix, message.toHtmlEscaped());

    m_textEdit->appendHtml(html);

    // Auto-scroll to bottom.
    auto* sb = m_textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogConsole::clear() {
    m_textEdit->clear();
}

void LogConsole::onClear() {
    clear();
}
