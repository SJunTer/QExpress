#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include "../network/socket.h"

#define PORTNO 6666

class LoginDlg : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDlg(QWidget *parent = 0);
    ClientSocket *client() { return cli; }

private:
    ClientSocket *cli;
    QPushButton *signInBtn;
    QPushButton *signUpBtn;
    QLabel *usrLabel;
    QLabel *pwLabel;
    QLineEdit *usrEdit;
    QLineEdit *pwdEdit;

    int testUsrPw(const QString &usr, const QString &pw);

private slots:
    void signIn();
    void signUp();

};


#endif // LOGINDLG_H
