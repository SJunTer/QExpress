#ifndef TASKDLG_H
#define TASKDLG_H

#include <QDialog>
#include <QList>
#include <QStringList>
#include "deliverypath.h"

QT_BEGIN_NAMESPACE
class ClientSocket;
class QTimer;
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

    QTimer *timer;

    int getTask();
    void updateUi();

signals:
    void drawPath(QList<Place> &ps);
    void updatePath(int pos);
    void endTask();
    void setTaskState(bool b);

private slots:
    void on_acptBtn_clicked();
    void on_endBtn_clicked();
    void updatePath();

public slots:
    void update();  // 更新任务
};

#endif // TASKDLG_H
