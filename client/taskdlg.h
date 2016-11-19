#ifndef TASKDLG_H
#define TASKDLG_H

#include <QDialog>
#include <QStringList>


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
    QStringList cargos;
    int truckId;
    bool accepted;
    bool taskLoaded;

    int getTask();
    void updateUi();

signals:
    void acptTask();

private slots:
    void on_acptBtn_clicked();

public slots:
    void update();
};

#endif // TASKDLG_H
