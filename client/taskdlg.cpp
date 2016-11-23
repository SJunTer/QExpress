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
    string data;
    commandType cmd;
    data += toByteString(0);
    if(sendPacket(client->sock(), cmd_getTask, data) != 0)
        return SEND_ERROR;
    if(recvPacket(client->sock(), &cmd, data) != 0)
        return RECV_ERROR;

    int flag = fromByteString<int>(data);
    if(flag == 0)
        return NO_TASK;

    // 接收车辆编号
    path.truckId = fromByteString<int>(data);
    // 接收配送点信息
    int cnt = fromByteString<int>(data), len;
    string s;
    Place p;
    double x, y;
    for(int i = 0; i < cnt; ++i)
    {
        x = fromByteString<double>(data);
        y = fromByteString<double>(data);
        p.coord = QPointF(x, y);
        len = fromByteString<int>(data);
        s = fromByteString(data, len);
        p.title = QString::fromStdString(s);
        p.type = PlaceType(fromByteString<int>(data));
        path.places.push_back(p);
    }
    // 接收货物清单
    string cargo;
    path.cargos.clear();
    cnt = fromByteString<int>(data);
    for(int i = 0; i < cnt; ++i)
    {
        int len = fromByteString<int>(data);
        cargo = fromByteString(data, len);
        path.cargos.push_back(QString::fromStdString(cargo));
    }
    return 0;
}

void TaskDlg::updateUi()
{
    ui->cargoListWidget->clear();
    ui->cargoListWidget->addItems(path.cargos);
    ui->truckLabel->setText(QString::number(path.truckId));
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
    emit sendPath(path.places);
    QMessageBox::information(this, "成功", "已接受配送任务", QMessageBox::Ok);
    ui->acptBtn->setEnabled(false);
    ui->endBtn->setEnabled(true);
    accepted = true;
}

// 任务失败
void TaskDlg::on_endBtn_clicked()
{
    if(QMessageBox::warning(this, "警告", "您确定结束当前任务吗？",
                            QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    string s = toByteString(0);
    if(sendPacket(client->sock(), cmd_taskFail, s) != 0)
    {
        QMessageBox::warning(this, "错误", "数据传输失败", QMessageBox::Ok);
        return;
    }
    QMessageBox::information(this, "提示", "已结束任务", QMessageBox::Ok);
    emit endTask();
    ui->acptBtn->setEnabled(true);
    ui->endBtn->setEnabled(false);
    taskLoaded = false;
    accepted = false;
}


//更新任务
void TaskDlg::update()
{
    if(accepted || taskLoaded)
        return;

    if(getTask() == 0)
    {
        updateUi();
        taskLoaded = true;
    }
    else if(getTask() == NO_TASK)
    {
        QMessageBox::warning(this, "提示", "无配送任务", QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, "错误", "数据传输异常！", QMessageBox::Ok);
    }
}
