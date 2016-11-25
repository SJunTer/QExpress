#include "userdlg.h"
#include "ui_userdlg.h"
#include "pwddlg.h"
#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include <QMessageBox>
#include <string>
#include <QDebug>
using namespace std;

UserDlg::UserDlg(ClientSocket *cli, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDlg),
    client(cli)
{
    ui->setupUi(this);
    if(getUserInfo() != 0)
        QMessageBox::warning(this, "错误", "无法获取用户信息", QMessageBox::Ok);
}

UserDlg::~UserDlg()
{
    delete ui;
}

int UserDlg::getUserInfo()
{
    string data = toByteString(0);
    commandType cmd;
    if(sendPacket(client->sock(), cmd_getUserInfo, data) != 0)
        return SEND_ERROR;
    if(recvPacket(client->sock(), &cmd, data) != 0)
        return RECV_ERROR;

    int len = fromByteString<int>(data);
    acc.username = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    acc.password = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    acc.name = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    acc.email = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    acc.phone = QString::fromStdString(fromByteString(data, len));
    ui->usrLineEdit->setText(acc.username);
    ui->pwdLineEdit->setText("******");
    ui->nameLineEdit->setText(acc.name);
    ui->emailLineEdit->setText(acc.email);
    ui->phoneLineEdit->setText(acc.phone);

    return 0;
}

void UserDlg::on_usrEditBtn_clicked()
{
    ui->usrLineEdit->setReadOnly(false);
}

// 密码弹窗修改
void UserDlg::on_pwdEditBtn_clicked()
{
    PwdDlg *pwDlg = new PwdDlg(acc.username, client, this);
    pwDlg->setAttribute(Qt::WA_DeleteOnClose);
    pwDlg->exec();
}

void UserDlg::on_nameEditBtn_clicked()
{
    ui->nameLineEdit->setReadOnly(false);
}

void UserDlg::on_emailEditBtn_clicked()
{
    ui->emailLineEdit->setReadOnly(false);
}

void UserDlg::on_phoneEditBtn_clicked()
{
    ui->phoneLineEdit->setReadOnly(false);
}

// 确定修改
void UserDlg::on_okBtn_clicked()
{
    if(!ui->nameLineEdit->isModified() && !ui->phoneLineEdit->isModified()
            && !ui->emailLineEdit->isModified())
    {
        emit closed();
        this->close();
        return;
    }
    if(QMessageBox::warning(this, "提示", "确定提交修改信息？", QMessageBox::Yes|QMessageBox::No)
            == QMessageBox::No)
    {
        return;
    }
    string data, s;
    int len;
    s = ui->nameLineEdit->text().toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = ui->emailLineEdit->text().toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = ui->phoneLineEdit->text().toStdString();
    len = s.size();
    data += toByteString(len) + s;
    if(sendPacket(client->sock(), cmd_changeUserInfo, data) != 0)
    {
        QMessageBox::information(this, "错误", "修改失败，请检查网络连接！", QMessageBox::Ok);
        return;
    }

    QMessageBox::information(this, "成功", "修改成功！", QMessageBox::Ok);

    emit closed();
    this->close();
}

void UserDlg::on_cancelBtn_clicked()
{    if(ui->nameLineEdit->isModified() || ui->phoneLineEdit->isModified()
            || ui->emailLineEdit->isModified())
    {
        if(QMessageBox::warning(this, "警告", "退出将放弃当前修改信息，是否继续？",
                                 QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        {
            return;
        }
    }
    emit closed();
     this->close();
}
