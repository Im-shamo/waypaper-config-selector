#include "mainwindow.h"
#include "QtLogging"


#include <QApplication>
#include "config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Config config;
    MainWindow w(config, 5);
    w.show();
    return a.exec();
}
