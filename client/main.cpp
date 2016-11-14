#include <QApplication>
#include "mainwindow.h"
#include "logindlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginDlg dlg;
    if(dlg.exec() == QDialog::Accepted)
    {
        MainWindow w(0, dlg.client());
        w.show();
        return a.exec();
    }
    return 0;
}
