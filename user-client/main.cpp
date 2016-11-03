#include <QApplication>
#include "mainwindow.h"
#include "logindlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginDlg dlg;

    MainWindow w;
    w.show();

    return a.exec();
}
