#include "VisionApp/MainWindow.h"
#include "VisionApp/ImageViewer.h"
#include "VisionApp/LogConsole.h"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

// ── Construction ────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Vision Studio");
    setMinimumSize(1024, 720);
    resize(1400, 900);

    // Central widget — image viewer.
    m_imageViewer = new ImageViewer(this);
    setCentralWidget(m_imageViewer);

    // Build UI.
    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
    createDockWidgets();

    // Load VisionCore dynamically.
    if (!loadVisionCore()) {
        statusBar()->showMessage("Warning: VisionCore library not found — engine unavailable.", 5000);
    }
}

MainWindow::~MainWindow() {
    if (m_engine) {
        m_engine->shutdown();
    }
}

// ── Actions ─────────────────────────────────────────────────────────────────

void MainWindow::createActions() {
    m_actOpen = new QAction(tr("&Open Image..."), this);
    m_actOpen->setShortcut(QKeySequence::Open);
    m_actOpen->setStatusTip(tr("Open an image file"));
    connect(m_actOpen, &QAction::triggered, this, &MainWindow::onOpenImage);

    m_actPreprocess = new QAction(tr("&Preprocess"), this);
    m_actPreprocess->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    m_actPreprocess->setStatusTip(tr("Run preprocessing pipeline on current image"));
    m_actPreprocess->setEnabled(false);
    connect(m_actPreprocess, &QAction::triggered, this, &MainWindow::onPreprocess);

    m_actRunOcr = new QAction(tr("Run &OCR"), this);
    m_actRunOcr->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    m_actRunOcr->setStatusTip(tr("Run OCR on preprocessed image"));
    m_actRunOcr->setEnabled(false);
    connect(m_actRunOcr, &QAction::triggered, this, &MainWindow::onRunOcr);

    m_actAbout = new QAction(tr("&About"), this);
    connect(m_actAbout, &QAction::triggered, this, &MainWindow::onAbout);

    m_actQuit = new QAction(tr("&Quit"), this);
    m_actQuit->setShortcut(QKeySequence::Quit);
    connect(m_actQuit, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::createMenus() {
    auto* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_actOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actQuit);

    auto* processMenu = menuBar()->addMenu(tr("&Process"));
    processMenu->addAction(m_actPreprocess);
    processMenu->addAction(m_actRunOcr);

    auto* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(m_actAbout);
}

void MainWindow::createToolBar() {
    m_toolBar = addToolBar(tr("Main Toolbar"));
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(24, 24));

    m_toolBar->addAction(m_actOpen);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actPreprocess);
    m_toolBar->addAction(m_actRunOcr);
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWidgets() {
    // Log Console dock.
    auto* logDock = new QDockWidget(tr("Log Console"), this);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);

    m_logConsole = new LogConsole(logDock);
    logDock->setWidget(m_logConsole);

    addDockWidget(Qt::BottomDockWidgetArea, logDock);

    // Add toggle to View menu.
    auto* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(logDock->toggleViewAction());
}

// ── Dynamic Loading ─────────────────────────────────────────────────────────

bool MainWindow::loadVisionCore() {
    // Try to load VisionCore from the same directory as the executable.
#ifdef _WIN32
    m_coreLib.setFileName("VisionCore");
#elif defined(__APPLE__)
    m_coreLib.setFileName("libVisionCore");
#else
    m_coreLib.setFileName("libVisionCore");
#endif

    if (!m_coreLib.load()) {
        m_logConsole->appendMessage(3, "Failed to load VisionCore: " + m_coreLib.errorString());
        return false;
    }

    // Resolve factory function.
    auto createFunc = reinterpret_cast<CreateEngineFunc>(
        m_coreLib.resolve("createVisionEngine"));

    if (!createFunc) {
        m_logConsole->appendMessage(3, "Failed to resolve createVisionEngine()");
        return false;
    }

    m_engine.reset(createFunc());

    // Wire up the log callback to our LogConsole.
    m_engine->setLogCallback(
        [this](vision::LogLevel level, const std::string& msg) {
            // QMetaObject::invokeMethod is thread-safe for cross-thread calls.
            QMetaObject::invokeMethod(m_logConsole, [this, level, msg]() {
                m_logConsole->appendMessage(static_cast<int>(level),
                                            QString::fromStdString(msg));
            });
        }
    );

    if (!m_engine->initialize()) {
        m_logConsole->appendMessage(3, "VisionCore engine failed to initialize.");
        m_engine.reset();
        return false;
    }

    m_logConsole->appendMessage(1, "VisionCore loaded successfully.");
    return true;
}

// ── Slots ───────────────────────────────────────────────────────────────────

void MainWindow::onOpenImage() {
    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open Image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.tiff *.tif);;All Files (*)")
    );

    if (path.isEmpty()) return;

    m_currentImagePath = path;

    if (m_engine && m_engine->loadImage(path.toStdString())) {
        updateImageDisplay();
        m_actPreprocess->setEnabled(true);
        m_actRunOcr->setEnabled(true);
        statusBar()->showMessage(tr("Loaded: %1").arg(path));
    } else {
        // Fallback: display directly if engine unavailable.
        QImage img(path);
        if (!img.isNull()) {
            m_imageViewer->setImage(img);
            statusBar()->showMessage(tr("Loaded (no engine): %1").arg(path));
        } else {
            statusBar()->showMessage(tr("Failed to load image."));
        }
    }
}

void MainWindow::onPreprocess() {
    if (!m_engine) return;

    vision::PreprocessOptions opts;
    opts.grayscale          = true;
    opts.threshold          = true;
    opts.perspectiveCorrect = false;

    if (m_engine->preprocess(opts)) {
        updateImageDisplay();
        statusBar()->showMessage(tr("Preprocessing complete."));
    } else {
        statusBar()->showMessage(tr("Preprocessing failed."));
    }
}

void MainWindow::onRunOcr() {
    if (!m_engine) return;

    auto results = m_engine->runOcr();

    if (results.empty()) {
        m_logConsole->appendMessage(2, "OCR returned no results.");
        statusBar()->showMessage(tr("OCR: no text detected."));
    } else {
        for (const auto& r : results) {
            QString msg = QString("OCR [%.2f]: \"%1\"")
                .arg(QString::fromStdString(r.text));
            m_logConsole->appendMessage(1, msg);
        }
        statusBar()->showMessage(
            tr("OCR: %1 region(s) detected.").arg(results.size()));
    }
}

void MainWindow::onAbout() {
    QString ver = m_engine ? QString::fromStdString(m_engine->versionString())
                           : "Engine not loaded";
    QMessageBox::about(this, tr("About Vision Studio"),
        tr("<h2>Vision Studio</h2>"
           "<p>Cross-platform OCR &amp; Image Processing</p>"
           "<p>Engine: %1</p>"
           "<p>Built with Qt %2, OpenCV 4</p>")
            .arg(ver, QT_VERSION_STR));
}

// ── Helpers ─────────────────────────────────────────────────────────────────

void MainWindow::updateImageDisplay() {
    if (!m_engine) return;

    int w = 0, h = 0, ch = 0;
    const uint8_t* data = m_engine->getImageData(w, h, ch);

    if (data && w > 0 && h > 0) {
        m_imageViewer->setImageFromData(data, w, h, ch);
    }
}
