#include "commands.h"
#include "packet.h"
#include "socket.h"
#include "logindlg.h"
#include "regdlg.h"
#include <string>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QBoxLayout>
#include <QDebug>

LoginDlg::LoginDlg(QWidget *parent)
    : QDialog(parent)
{
    cli = new ClientSocket;
    signInBtn = new QPushButton(tr("登陆"), this);
    signUpBtn = new QPushButton(tr("注册"), this);
    usrLabel = new QLabel(tr("用户名："), this);
    pwLabel = new QLabel(tr("密   码："), this);
    usrEdit = new QLineEdit(this);
    pwdEdit = new QLineEdit(this);

    signInBtn->setFixedSize(80, 30);
    signUpBtn->setFixedSize(80, 30);
    usrEdit->setFixedSize(250, 25);
    pwdEdit->setFixedSize(250, 25);
    pwdEdit->setEchoMode(QLineEdit::Password);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(usrLabel,0,0);
    gridLayout->addWidget(pwLabel,1,0);
    gridLayout->addWidget(usrEdit,0,1);
    gridLayout->addWidget(pwdEdit,1,1);
    gridLayout->setSpacing(8);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(0);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->addWidget(signInBtn);
    btnLayout->addWidget(signUpBtn);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(60,30,60,20);
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    setFixedSize(400,250);
    setWindowFlags(Qt::Widget);
    setWindowTitle(tr("QExpress"));

    connect(signInBtn, SIGNAL(clicked(bool)), this, SLOT(signIn()));
    connect(signUpBtn, SIGNAL(clicked(bool)), this, SLOT(signUp()));
}

int LoginDlg::testUsrPw(const QString &usr, const QString &pwd)
{
    std::string data;
    commandType cmd;
    data += toByteString(usr.size()) + usr.toStdString().c_str();
    data += toByteString(pwd.size()) + pwd.toStdString().c_str();
    if(sendPacket(cli->sock(), cmd_signIn, data) != 0)
        return SEND_ERROR;
    if(recvPacket(cli->sock(), &cmd, data) != 0)
        return RECV_ERROR;
    int ret = fromByteString<int>(data);
    return ret;
}

//登陆
void LoginDlg::signIn()
{
    // 判断输入是否未空
    if(usrEdit->text() == "")
    {
        QMessageBox::warning(this, tr("错误"), tr("请输入用户名！"), QMessageBox::Ok);
        usrEdit->setFocus();
        cli->closeSock();
        return;
    }
    if(pwdEdit->text() == "")
    {
        QMessageBox::warning(this, tr("错误"), tr("请输入密码！"), QMessageBox::Ok);
        pwdEdit->setFocus();
        cli->closeSock();
        return;
    }
    // 连接服务器
    if(cli->init(PORTNO, "192.168.1.107") != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("网络模块初始化失败！"), QMessageBox::Ok);
        signInBtn->setFocus();
        cli->closeSock();
        return;
    }
    if(cli->connectTo() != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法连接服务器！"), QMessageBox::Ok);
        signInBtn->setFocus();
        cli->closeSock();
        return;
    }
    // 验证帐号密码
    int ret = testUsrPw(usrEdit->text(), pwdEdit->text());
    if(ret == SEND_ERROR || ret == RECV_ERROR)
    {
        QMessageBox::warning(this, tr("错误"), tr("数据传输错误，请检查网络连接！"), QMessageBox::Ok);
        usrEdit->setFocus();
        cli->closeSock();
        return;
    }
    else if(ret == LOGIN_NO)
    {
        QMessageBox::warning(this, tr("错误"), tr("用户名密码不匹配！"), QMessageBox::Ok);
        usrEdit->setFocus();
        cli->closeSock();
        return;
    }
    QDialog::accept();
}

//注册
void LoginDlg::signUp()
{

}
