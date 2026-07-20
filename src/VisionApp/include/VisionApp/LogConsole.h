#pragma once

#include <QWidget>

class QPlainTextEdit;

// ── LogConsole ──────────────────────────────────────────────────────────────
/**
 * @brief Read-only log output widget that receives messages from VisionCore.
 *
 * Colour-coded by severity level: Debug (gray), Info (white), Warning
 * (yellow), Error (red).
 */
class LogConsole : public QWidget {
    Q_OBJECT

public:
    explicit LogConsole(QWidget* parent = nullptr);
    ~LogConsole() override = default;

    /**
     * @brief Append a log message with colour based on severity.
     * @param level   0=Debug, 1=Info, 2=Warning, 3=Error.
     * @param message Message text to display.
     */
    void appendMessage(int level, const QString& message);

    /**
     * @brief Clear all log output.
     */
    void clear();

public slots:
    void onClear();

private:
    QPlainTextEdit* m_textEdit = nullptr;
};
