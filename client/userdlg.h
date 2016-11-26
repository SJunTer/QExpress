#ifndef USERDLG_H
#define USERDLG_H

#include <QDialog>
#include "account.h"

QT_BEGIN_NAMESPACE
class ClientSocket;
QT_END_NAMESPACE


namespace Ui {
class UserDlg;
}


class UserDlg : public QDialog
{
    Q_OBJECT

public:
    explicit UserDlg(ClientSocket *cli, QWidget *parent = 0);
    ~UserDlg();

private:
    Ui::UserDlg *ui;

    Account acc;
    ClientSocket *client;

    int getUserInfo();

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void closed();

private slots:
    void on_usrEditBtn_clicked();
    void on_pwdEditBtn_clicked();
    void on_nameEditBtn_clicked();
    void on_emailEditBtn_clicked();
    void on_phoneEditBtn_clicked();
    void on_okBtn_clicked();
    void on_cancelBtn_clicked();

};

#endif // USERDLG_H
