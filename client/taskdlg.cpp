#include "taskdlg.h"
#include "ui_taskdlg.h"
#include "../network/commands.h"
#include "../network/packet.h"
#include "../network/socket.h"
#include <QTimer>
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
    timer = new QTimer(this);
}

TaskDlg::~TaskDlg()
{
    delete ui;
}

//更新任务信息
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
    path.places.clear();
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
    path.pos = path.secs = 0;
    ui->acptBtn->setEnabled(true);

    return 0;
}

void TaskDlg::updateUi()
{
    ui->placeListWidget->clear();
    for(int i = 0; i < path.places.size(); ++i)
    {
        if(path.places[i].type != IsPass)
            ui->placeListWidget->addItem(path.places[i].title);
    }
    ui->cargoListWidget->clear();
    ui->cargoListWidget->addItems(path.cargos);
    ui->truckLabel->setText(QString::number(path.truckId));
}



//开始配送
void TaskDlg::on_acptBtn_clicked()
{
    if(!taskLoaded)
    {
        QMessageBox::information(this, "提示", "当前无配送任务", QMessageBox::Ok);
        return;
    }
    emit drawPath(path.places);
    emit setTaskState(true);
    // 模拟行车路线
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePath()));
    timer->start(1000);

    string s = toByteString(0);
    if(sendPacket(client->sock(), cmd_acceptTask, s) != 0)
    {
        QMessageBox::warning(this, "错误", "数据传输失败", QMessageBox::Ok);
        return;
    }
    QMessageBox::information(this, "成功", "开始配送", QMessageBox::Ok);
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

    emit endTask();
    emit setTaskState(false);
    timer->stop();

    string s = toByteString(0);
    if(sendPacket(client->sock(), cmd_taskFail, s) != 0)
    {
        QMessageBox::warning(this, "错误", "数据传输失败", QMessageBox::Ok);
        return;
    }
    QMessageBox::information(this, "提示", "已结束任务", QMessageBox::Ok);

    ui->endBtn->setEnabled(false);
    taskLoaded = false;
    accepted = false;
}

//更新路径
void TaskDlg::updatePath()
{
    path.pos += 1;
    path.secs += 2;
    emit updatePath(path.pos);

    string s = toByteString(path.pos);
    if(sendPacket(client->sock(), cmd_posChange, s) != 0)
    {
        QMessageBox::warning(this, "错误", "数据传输失败", QMessageBox::Ok);
        return;
    }
    if(path.pos == path.places.size()-1)    // 如果到达终点
    {
        timer->stop();
        emit setTaskState(false);
        ui->endBtn->setEnabled(false);
        taskLoaded = false;
        accepted = false;
        QMessageBox::information(0, "提示", "任务完成！", QMessageBox::Ok);

        s = toByteString(0);
        if(sendPacket(client->sock(), cmd_taskFinish, s) != 0)
        {
            QMessageBox::warning(this, "错误", "数据传输失败", QMessageBox::Ok);
            return;
        }
    }
}

