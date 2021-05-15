#include "mainwindow.h"
#include "stdafx.h"
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mw;
    mw.show();
    return a.exec();
}
