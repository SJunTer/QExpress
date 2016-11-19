#include "uploaddlg.h"
#include "ui_uploaddlg.h"
#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include <string>
#include <QButtonGroup>
#include <QMessageBox>
#include <QDebug>

using namespace std;

UploadDlg::UploadDlg(ClientSocket *cli, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadDlg),
    client(cli),
    type(-1)
{
    ui->setupUi(this);
    setWindowTitle("上报");
    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->accRadioBtn, ACCIDENT);
    buttonGroup->addButton(ui->jamRadioBtn, JAM);
    buttonGroup->addButton(ui->blockRadioBtn, BLOCK);
}

UploadDlg::~UploadDlg()
{
    delete ui;
}


void UploadDlg::on_submitBtn_clicked()
{
    int type = buttonGroup->checkedId();
    address = ui->addrLineEdit->text();
    detail = ui->detailText->toPlainText();
    if(type == -1)
    {
        QMessageBox::warning(this, "提示", "请选择路况类型", QMessageBox::Ok);
        return;
    }
    if(address == "")
    {
        QMessageBox::warning(this, "提示", "请填写地址", QMessageBox::Ok);
        return;
    }
    if(detail == "")
    {
        QMessageBox::warning(this, "提示", "请填写详细情况", QMessageBox::Ok);
        return;
    }
    string s;
    s += toByteString(type);
    s += toByteString(address.size()) + address.toStdString();
    s += toByteString(detail.size()) + detail.toStdString();
    if(sendPacket(client->sock(), cmd_submitRoad, s) != 0)
    {
        QMessageBox::warning(this, "提示", "数据传输错误", QMessageBox::Ok);
        return;
    }
    QMessageBox::warning(this, "成功", "上报成功，感谢您的反馈！", QMessageBox::Ok);

}

void UploadDlg::on_cancelBtn_clicked()
{
    emit closed();
    this->close();
}
