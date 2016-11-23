#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include "regdlg.h"
#include "ui_regdlg.h"
#include <string>
#include <QMessageBox>

using namespace std;

RegDlg::RegDlg(int p, QString &i, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegDlg)
    , port(p)
    , ip(i)
{
    client = new ClientSocket;
    ui->setupUi(this);
    ui->usrLineEdit->setFocus();
    // 设置控件切换顺序
    setTabOrder(ui->usrLineEdit, ui->pwdLineEdit);
    setTabOrder(ui->pwdLineEdit, ui->pwd2LienEdit);
    setTabOrder(ui->pwd2LienEdit, ui->nameLineEdit);
    setTabOrder(ui->nameLineEdit, ui->emailLineEdit);
    setTabOrder(ui->emailLineEdit, ui->phoneLineEdit);
    setTabOrder(ui->phoneLineEdit, ui->agreeBox);
    setTabOrder(ui->agreeBox, ui->regBtn);
    setTabOrder(ui->regBtn, ui->cancelBtn);
    setTabOrder(ui->cancelBtn, ui->usrLineEdit);
    setWindowTitle("注册");
}

RegDlg::~RegDlg()
{
    delete ui;
    delete client;
}


void RegDlg::on_regBtn_clicked()
{
    // 检查信息是否完整
    if((acc.username = ui->usrLineEdit->text()) == "" ||
            (acc.password = ui->pwdLineEdit->text()) == "" ||
            (acc.name = ui->nameLineEdit->text()) == "" ||
            (acc.email = ui->emailLineEdit->text()) == "" ||
            (acc.phone = ui->phoneLineEdit->text()) == "")
    {
        QMessageBox::warning(this, "提示", "请填写完整信息！", QMessageBox::Ok);
        return;
    }
    if(ui->pwdLineEdit->text() != ui->pwd2LienEdit->text())
    {
        QMessageBox::warning(this, "提示", "两次填写密码不一致！", QMessageBox::Ok);
        return;
    }

    // 连接服务器
    if(client->init(port, ip.toStdString().c_str()) != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("网络模块初始化失败！"), QMessageBox::Ok);
        client->closeSock();
        return;
    }
    if(client->connectTo() != 0)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法连接服务器！"), QMessageBox::Ok);
        client->closeSock();
        return;
    }
    std::string data;
    commandType cmd;
    string s;
    int len;
    s = acc.username.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.password.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.name.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.phone.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.email.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    if(sendPacket(client->sock(), cmd_signUp, data) != 0)
    {
        QMessageBox::warning(this, "错误", "注册失败，请检查服务器连接！", QMessageBox::Ok);
        return;
    }
    if(recvPacket(client->sock(), &cmd, data) != 0)
    {
        QMessageBox::warning(this, "错误", "注册失败，请检查服务器连接！", QMessageBox::Ok);
        return;
    }
    int ret = fromByteString<int>(data);
    if(ret == USR_REPEAT)
    {
        QMessageBox::warning(this, "错误", "用户名已存在！", QMessageBox::Ok);
        return;
    }
    else
    {
        QMessageBox::warning(this, "成功", "注册信息已提交，请等待审核", QMessageBox::Ok);
        client->closeSock();
        this->close();
        return;
    }
}
