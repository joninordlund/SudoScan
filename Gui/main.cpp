#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

void loadStyleSheet()
{
#ifdef QT_DEBUG
    QString stylePath = QFileInfo(__FILE__).absolutePath() + "/style.css";
#else
    QString stylePath = ":/style.css";
#endif
    QFile file(stylePath);
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        qApp->setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }
    else
    {
        qWarning() << "Couldn't open the style file:" << stylePath;
    }
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("SudoScan");
    QCoreApplication::setApplicationName("SudoScan");

    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Bold.ttf");
    loadStyleSheet();

    MainWindow window;
    window.setObjectName("center");
    window.show();

    return app.exec();
}
