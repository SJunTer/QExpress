#include "taskdlg.h"
#include "ui_taskdlg.h"
#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include <string>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>

using namespace std;

TaskDlg::TaskDlg(ClientSocket *cli, QWidget *parent)  :
    QDialog(parent),
    ui(new Ui::TaskDlg),
    client(cli),
    truckId(-1),
    accepted(false),
    taskLoaded(false)
{
    ui->setupUi(this);
    setWindowTitle("任务");
}

TaskDlg::~TaskDlg()
{
    delete ui;
}

int TaskDlg::getTask()
{
    string s;
    commandType cmd;
    s += toByteString(0);
    if(sendPacket(client->sock(), cmd_getTask, s) != 0)
        return SEND_ERROR;
    if(recvPacket(client->sock(), &cmd, s) != 0)
        return RECV_ERROR;
    qDebug() << s.size();
    // 接收货物清单
    string cargo;
    int cnt = fromByteString<int>(s);
    for(int i = 0; i < cnt; ++i)
    {
        int len = fromByteString<int>(s);
        cargo = fromByteString(s, len);
        cargos.push_back(QString::fromStdString(cargo));
    }
    // 接收车辆编号
    truckId = fromByteString<int>(s);
    return 0;
}

void TaskDlg::updateUi()
{
    ui->cargoListWidget->addItems(cargos);
    ui->truckLabel->setText(QString::number(truckId));
}

//响应按钮点击事件
void TaskDlg::on_acptBtn_clicked()
{
    if(!taskLoaded)
    {
        QMessageBox::information(this, "提示", "当前无配送任务", QMessageBox::Ok);
        return;
    }
    string s = toByteString(0);
    if(sendPacket(client->sock(), cmd_acceptTask, s) != 0)
    {
        QMessageBox::warning(this, "错误", "数据传输失败", QMessageBox::Ok);
        return;
    }
    emit acptTask();
    QMessageBox::information(this, "成功", "已接受配送任务", QMessageBox::Ok);
    ui->acptBtn->setEnabled(false);
    accepted = true;
}

//更新任务
void TaskDlg::update()
{
    if(accepted)
        return;

    if(getTask() != 0)
    {
        QMessageBox::warning(this, "错误", "数据传输异常！", QMessageBox::Ok);
        return;
    }
    updateUi();
    taskLoaded = true;
}
