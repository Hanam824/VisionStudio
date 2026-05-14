#include "VisionApp/MainWindow.h"

#include <QApplication>
#include <QFile>
#include <QDir>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Application metadata.
    QApplication::setApplicationName("Vision Studio");
    QApplication::setOrganizationName("VisionStudio");
    QApplication::setApplicationVersion("0.1.0");

    // Load dark theme stylesheet.
    {
        QFile styleFile(":/styles/dark_theme.qss");
        if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
            app.setStyleSheet(styleFile.readAll());
            styleFile.close();
        }
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
