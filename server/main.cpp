#include <QApplication>
#include "mainwindow.h"
#include "actdlg.h"
#include <QMessageBox>
#include "../activate/activate.h"
#include "database/operatesql.h"
#include "../network/socket.h"

#define PORT 6666


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Activate act;
    if(!act.KeyIsCorrect())
    {
        ActDlg dlg(&act);
        if(dlg.exec() == QDialog::Rejected)
            return 0;
    }
    ServerSocket server;
    if(server.init(PORT) == 0)
    {
        Dbsql dbsql;
        if(dbsql.Connetion("", "root", "Qaz8864352!", "test", 10) == 0)
            //         if(dbsql.Connetion("171.34.188.215", "pig", "xy123.", "my_SQL", 10) == 0)
        {
            MainWindow w(&server, &dbsql);
            w.show();
            return a.exec();
        }
        QMessageBox::information(0, "错误", "无法连接数据库", QMessageBox::Ok);
        return 0;
    }
    QMessageBox::information(0, "错误", "无法启动服务器", QMessageBox::Ok);

    return 0;
}
