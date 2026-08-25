#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("SudoScan");
    QCoreApplication::setApplicationName("SudoScan");

    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto-Bold.ttf");

    MainWindow window;
    window.setObjectName("center");
    window.show();

    return app.exec();
}

