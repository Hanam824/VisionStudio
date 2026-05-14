#pragma once

#include "VisionCore/IVisionEngine.h"

#include <QMainWindow>
#include <QLibrary>

#include <memory>

class QAction;
class QLabel;
class QStatusBar;
class QToolBar;

class ImageViewer;
class LogConsole;

// ── MainWindow ──────────────────────────────────────────────────────────────
/// Top-level application window.
/// Dynamically loads VisionCore and provides toolbar, image viewer, and log
/// console.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOpenImage();
    void onPreprocess();
    void onRunOcr();
    void onAbout();

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createStatusBar();
    void createDockWidgets();

    bool loadVisionCore();
    void updateImageDisplay();

    // ── UI Widgets ──────────────────────────────────────────────────────
    ImageViewer* m_imageViewer  = nullptr;
    LogConsole*  m_logConsole   = nullptr;
    QToolBar*    m_toolBar      = nullptr;

    // ── Actions ─────────────────────────────────────────────────────────
    QAction* m_actOpen       = nullptr;
    QAction* m_actPreprocess = nullptr;
    QAction* m_actRunOcr     = nullptr;
    QAction* m_actAbout      = nullptr;
    QAction* m_actQuit       = nullptr;

    // ── Engine ──────────────────────────────────────────────────────────
    QLibrary                              m_coreLib;
    std::unique_ptr<vision::IVisionEngine> m_engine;
    QString                               m_currentImagePath;
};
