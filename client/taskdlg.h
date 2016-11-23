#ifndef TASKDLG_H
#define TASKDLG_H

#include <QDialog>
#include <QList>
#include <QStringList>
#include "deliverypath.h"

QT_BEGIN_NAMESPACE
class ClientSocket;
QT_END_NAMESPACE


namespace Ui {
class TaskDlg;
}


class TaskDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDlg(ClientSocket *cli, QWidget *parent = 0);
    ~TaskDlg();

private:
    Ui::TaskDlg *ui;

    ClientSocket *client;
    DeliveryPath path;
    bool accepted;
    bool taskLoaded;

    int getTask();
    void updateUi();

signals:
    void acptTask();
    void sendPath(QList<Place> &ps);
    void endTask();

private slots:
    void on_acptBtn_clicked();
    void on_endBtn_clicked();

public slots:
    void update();  // 更新任务
};

#endif // TASKDLG_H
