#ifndef REGDLG_H
#define REGDLG_H

#include <QDialog>
#include <QString>
#include "account.h"

QT_BEGIN_NAMESPACE
class ClientSocket;
QT_END_NAMESPACE

namespace Ui {
class RegDlg;
}

class RegDlg : public QDialog
{
    Q_OBJECT
public:
    explicit RegDlg(int p, QString &i, QWidget *parent = 0);
    ~RegDlg();

private:
    Ui::RegDlg *ui;
    Account acc;
    ClientSocket *client;
    int port;
    QString ip;

private slots:
    void on_regBtn_clicked();
};

#endif // REGDLG_H
