#include "mainwindow.h"

#include <QtWidgets/QApplication>

int mainOfGui(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow   w;
    w.show();
    return a.exec();
}
