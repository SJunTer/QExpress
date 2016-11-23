#include "pwddlg.h"
#include "ui_pwddlg.h"
#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include <QMessageBox>
#include <string>
using namespace std;

PwdDlg::PwdDlg(QString &usr, ClientSocket *cli, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PwdDlg),
    usrnm(usr),
    client(cli)
{
    ui->setupUi(this);
}

PwdDlg::~PwdDlg()
{
    delete ui;
}

void PwdDlg::on_okBtn_clicked()
{
    if(ui->pw1LineEdit->text() == "" || ui->pw2LineEdit->text() == ""
            || ui->pw3LineEdit->text() == "")
    {
        QMessageBox::warning(this, "警告", "请填写完整信息！", QMessageBox::Ok);
        return;
    }
    if(ui->pw2LineEdit->text() != ui->pw3LineEdit->text())
    {
        QMessageBox::warning(this, "错误", "两次填写密码不一致！", QMessageBox::Ok);
        return;
    }
    string data, s;
    commandType cmd;
    int len;
    s = usrnm.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = ui->pw1LineEdit->text().toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = ui->pw2LineEdit->text().toStdString();
    len = s.size();
    data += toByteString(len) + s;

    if(sendPacket(client->sock(), cmd_changePwd, data) != 0)
    {
        QMessageBox::warning(this, "错误", "网络连接异常！", QMessageBox::Ok);
        return;
    }
    if(recvPacket(client->sock(), &cmd, data) != 0)
    {
        QMessageBox::warning(this, "错误", "网络连接异常！", QMessageBox::Ok);
        return;
    }
    int ret = fromByteString<int>(data);
    if(ret == PWD_WRONG)
    {
        QMessageBox::warning(this, "错误", "密码输入错误！", QMessageBox::Ok);
        return;
    }
    QMessageBox::warning(this, "成功", "密码修改成功！", QMessageBox::Ok);

    this->close();
}
