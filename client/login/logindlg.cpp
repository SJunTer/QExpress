#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include "logindlg.h"
#include "regdlg.h"
#include "settingdlg.h"
#include "ui_logindlg.h"
#include <string>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QBoxLayout>
#include <QDebug>

LoginDlg::LoginDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDlg)
    , ip("localhost")
    , port(PORTNO)
{
    ui->setupUi(this);
    cli = new ClientSocket;
    setWindowTitle("登录");
}

LoginDlg::~LoginDlg()
{
    delete ui;
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

int LoginDlg::sendRegInfo(Account &a)
{
    std::string data;
    commandType cmd;
    data += toByteString(a.username.size());
    data += a.username.toStdString().c_str();
    data += toByteString(a.password.size());
    data += a.password.toStdString().c_str();
    data += toByteString(a.name.size());
    data += a.name.toStdString().c_str();
    data += toByteString(a.phone.size());
    data += a.phone.toStdString().c_str();
    data += toByteString(a.email.size());
    data += a.email.toStdString().c_str();
    if(sendPacket(cli->sock(), cmd_signUp, data) != 0)
        return SEND_ERROR;
    if(recvPacket(cli->sock(), &cmd, data) != 0)
        return RECV_ERROR;
    int ret = fromByteString<int>(data);
    return ret;
}

// 登陆
void LoginDlg::on_loginBtn_clicked()
{
    // 判断输入是否未空
    if(ui->usrLineEdit->text() == "")
    {
        QMessageBox::warning(this, tr("错误"), tr("请输入用户名！"), QMessageBox::Ok);
        ui->usrLineEdit->setFocus();
        cli->closeSock();
        return;
    }
    if(ui->pwdLineEdit->text() == "")
    {
        QMessageBox::warning(this, tr("错误"), tr("请输入密码！"), QMessageBox::Ok);
        ui->pwdLineEdit->setFocus();
        cli->closeSock();
        return;
    }

    // 连接服务器
    if(cli->init(port, ip.toStdString().c_str()) != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("网络模块初始化失败！"), QMessageBox::Ok);
        ui->loginBtn->setFocus();
        cli->closeSock();
        return;
    }
    if(cli->connectTo() != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法连接服务器！"), QMessageBox::Ok);
        ui->loginBtn->setFocus();
        cli->closeSock();
        return;
    }

    // 验证帐号密码
    int ret = testUsrPw(ui->usrLineEdit->text(), ui->pwdLineEdit->text());
    if(ret == SEND_ERROR || ret == RECV_ERROR)
    {
        QMessageBox::warning(this, tr("错误"), tr("数据传输错误，请检查网络连接！"), QMessageBox::Ok);
        ui->loginBtn->setFocus();
        cli->closeSock();
        return;
    }
    else if(ret == LOGIN_NO)
    {
        QMessageBox::warning(this, tr("错误"), tr("用户名密码不匹配！"), QMessageBox::Ok);
        ui->usrLineEdit->setFocus();
        cli->closeSock();
        return;
    }
    QDialog::accept();
}

// 注册
void LoginDlg::on_signupBtn_clicked()
{
    RegDlg *dlg = new RegDlg(this);
    connect(dlg, SIGNAL(reg(Account&)), this, SLOT(reg(Account&)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

// 设置
void LoginDlg::on_toolButton_clicked()
{
    SettingDlg *dlg = new SettingDlg(ip, port, this);
    connect(dlg, SIGNAL(setInfo(QString&,int)), this, SLOT(setInfo(QString&,int)));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}


/*******************************
//---------------------公有槽函数--------------------------
*******************************/

//注册帐号
bool LoginDlg::reg(Account &a)
{
    // 连接服务器
    if(cli->init(port, ip.toStdString().c_str()) != 0)
    {
        cli->closeSock();
        return false;
    }
    if(cli->connectTo() != 0)
    {
        cli->closeSock();
        return false;
    }
    if(sendRegInfo(a) == REG_NO)    // 发送注册信息
    {
        cli->closeSock();
        return false;
    }
    else
    {
        cli->closeSock();
        return true;
    }
}

//设置网络信息
void LoginDlg::setInfo(QString &s, int p)
{
    ip = s;
    port = p;
}
