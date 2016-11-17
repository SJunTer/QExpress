#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QString>
#include "account.h"

#define PORTNO 6666

namespace Ui {
class LoginDlg;
}


QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QLineEdit;
class ClientSocket;
QT_END_NAMESPACE


class LoginDlg : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

    ClientSocket *client() { return cli; }

private:
    Ui::LoginDlg *ui;

    ClientSocket *cli;
    QString ip;
    int port;

    int testUsrPw(const QString &usr, const QString &pw);
    int sendRegInfo(Account &a);

private slots:
    void on_loginBtn_clicked();
    void on_toolButton_clicked();
    void on_signupBtn_clicked();

public slots:
    bool reg(Account &a);
    void setInfo(QString &s, int p);
};


#endif // LOGINDLG_H
