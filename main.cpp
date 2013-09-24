#include <QApplication>
#include <QDesktopWidget>

#include "gcpmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GCPMainWindow w;
    w.show();

    return app.exec();
}
