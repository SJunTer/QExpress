#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //登陆部分
    //......
    MainWindow w;
    w.show();

    return a.exec();
}
