#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QString>


#define PORTNO 6666

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
