#include"../network/packet.h"
#include "mapviewer/mapview.h"
#include "mapviewer/maplayer.h"
#include "deliverywidget.h"
#include "connection.h"
#include "database/operatedefine.h"
#include "database/operatesql.h"
#include <QGraphicsItem>
#include <QList>
#include <vector>
#include <string>
#include <cstdio>
#include <QDebug>

using namespace std;


Connection::Connection(Dbsql *d, int s, MapView *v) :
    sockfd(s),
    path(NULL),
    taskState(Task_none),
    view(v),
    dbsql(d),
    closed(false),
    login(false)
{
    qRegisterMetaType<Account>("Account");
}



/*************************************
 *                           公有槽函数                                 *
 * ************************************/
void Connection::start()
{
    string data;
    commandType cmd;

    while(1)
    {
        int ret = recvPacket(sockfd, &cmd, data);
        if(ret == CLIENT_CLOSE)
        {
            qDebug() << "CLIENT CLOSED";
            break;
        }
        else if(ret != 0)
        {
            qDebug() << "ERROR on receive";
            break;
        }
        switch(cmd)
        {
        case cmd_default: break;
        case cmd_signIn:    //用户登录
            signIn(data);
            break;
        case cmd_signUp:    //用户注册
            signUp(data);
            break;
        case cmd_getUserInfo:      //获得用户信息
            getUserInfo(data);
            break;
        case cmd_changeUserInfo:      //获得用户信息
            changeUserInfo(data);
            break;
        case cmd_changePwd:     // 验证密码
            changePwd(data);
            break;
        case cmd_preLoad:      //预加载地图信息
            preLoad(data);
            break;
        case cmd_getTile:      //获取切片
            getTile(data);
            break;
        case cmd_getSymbol: //获取地图标记
            getSymbol(data);
            break;
        case cmd_getTask:   //获得任务
            getTask(data);
            break;
        case cmd_acceptTask:  //接受任务
            acptTask(data);
            break;
        case cmd_posChange:  // 位置改变
            posChange(data);
            break;
        case cmd_taskFinish:  //任务结束
            taskFinish(data);
            break;
        case cmd_taskFail:  //终止任务
            taskFail(data);
            break;
        case cmd_submitRoad:    //提交路况
            submitRoad(data);
            break;
        default: break;
        }
    }
    if(!closed) //连接未断开
        emit close(sockfd);
    if(login)   // 注册连接不需要发送信号
        emit sg_signOut(acc.id);
    emit taskFinished();
}

// 服务器切断客户端连接
void Connection::stop()
{
    emit close(sockfd);
    closed = true;
}

void Connection::getTask(DeliveryPath *p)
{
    if(path->driverId == acc.id)
        path = p;
    taskState = Task_wait;
}



/*************************************
 *                           用户部分                                     *
 * ************************************/
// 验证用户名密码
int Connection::signIn(string &data)
{
    int usr_len = fromByteString<int>(data);
    string usr = fromByteString(data, usr_len);
    int pwd_len = fromByteString<int>(data);
    string pwd = fromByteString(data, pwd_len);

    int ret;
    if(dbsql->VerifyPWD(usr, pwd))
    {
        vector<string> info;
        dbsql->NormalSearch(TABLE_ACCOUNT, ACCOUNT_USERNAME, usr, info);
        if(stoi(info[7]) == OFFLINE)
            ret = LOGIN_YES;
        else
            ret = LOGIN_OL;
    }
    else
        ret = LOGIN_NO;

    data = toByteString(ret);
    if(sendPacket(sockfd, cmd_signIn, data) != 0)
        return SEND_ERROR;

    // 如果登录成功
    if(ret == LOGIN_YES)
    {
        // 获取用户信息
        vector<string> info;
        dbsql->NormalSearch(TABLE_ACCOUNT, ACCOUNT_USERNAME, usr, info);
        acc.id = stoi(info[0]);
        acc.username = QString::fromStdString(info[1]);
        acc.password = "";
        acc.name = QString::fromStdString(info[4]);
        acc.phone = QString::fromStdString(info[5]);
        acc.email = QString::fromStdString(info[6]);
        acc.status = ONLINE;
        acc.last_time = QTime::currentTime();
        emit sg_signIn(acc.id);
        login = true;
    }

    return 0;
}

//用户注册请求
int Connection::signUp(string &data)
{
    int str_len;
    str_len = fromByteString<int>(data);
    acc.username = QString::fromStdString(fromByteString(data, str_len));
    str_len = fromByteString<int>(data);
    acc.password = QString::fromStdString(fromByteString(data, str_len));
    str_len = fromByteString<int>(data);
    acc.name = QString::fromStdString(fromByteString(data, str_len));
    str_len = fromByteString<int>(data);
    acc.phone = QString::fromStdString(fromByteString(data, str_len));
    str_len = fromByteString<int>(data);
    acc.email = QString::fromStdString(fromByteString(data, str_len));

    int ret;
    // 判断帐号是否已存在
    vector<string> info;
    dbsql->NormalSearch(TABLE_ACCOUNT, ACCOUNT_USERNAME, acc.username.toStdString(), info);
    if(!info.empty())
        ret = USR_REPEAT;
    else
        ret = REG_OK;
    data = toByteString(ret);
    if(sendPacket(sockfd, cmd_signUp, data) != 0)
        return SEND_ERROR;

    if(ret == REG_OK)
        emit sg_signUp(acc);

    return 0;
}

// 请求用户信息
int Connection::getUserInfo(string &data)
{
    data.clear();
    int len;
    string s;
    s = acc.username.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.password.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.name.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.email.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    s = acc.phone.toStdString();
    len = s.size();
    data += toByteString(len) + s;
    if(sendPacket(sockfd, cmd_getUserInfo, data) != 0)
        return SEND_ERROR;

    return 0;
}


//更改用户信息
int Connection::changeUserInfo(string &data)
{
    int len = fromByteString<int>(data);
    acc.name = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    acc.email = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    acc.phone = QString::fromStdString(fromByteString(data, len));
    emit sg_changeInfo(acc);

    return 0;
}


int Connection::changePwd(string &data)
{
    int len = fromByteString<int>(data);
    string usr = fromByteString(data, len);
    len = fromByteString<int>(data);
    string pwd1 = fromByteString(data, len);
    len = fromByteString<int>(data);
    string pwd2 = fromByteString(data, len);

    int ret;
    if(dbsql->VerifyPWD(usr, pwd1))
    {
        if(dbsql->ModifyPWD(usr, pwd2) != 0)
            ret = PWD_WRONG;
        else
            ret = PWD_RIGHT;
    }
    else
        ret = PWD_WRONG;

    data = toByteString(ret);
    if(sendPacket(sockfd, cmd_changePwd, data) != 0)
        return SEND_ERROR;

    return 0;
}



/*************************************
 *                           地图部分                                     *
 * ************************************/
// 传递地图信息
int Connection::preLoad(string &data)
{
    data.clear();
    data += toByteString(view->originLevel);
    data += toByteString(view->sceneRect().x());
    data += toByteString(view->sceneRect().y());
    data += toByteString(view->sceneRect().width());
    data += toByteString(view->sceneRect().height());
    data += toByteString(view->originCenter.x());
    data += toByteString(view->originCenter.y());
    if(sendPacket(sockfd, cmd_signIn, data) != 0)
        return SEND_ERROR;

    return 0;
}

// 发送切片
int Connection::getTile(string &data)
{
    const int grid[] = { 10,10,10,10,10,10,10,12,16,23,32,45 };
    int level = fromByteString<int>(data);
    int row = fromByteString<int>(data);
    int col = fromByteString<int>(data);
    QString filename = QString("tiles/%1/%2_%3/%4_%5.png").arg(level).
            arg(row/grid[level-MIN_LEVEL]).arg(col/grid[level-MIN_LEVEL]).arg(row).arg(col);
    qDebug () << filename;

    data.clear();
    FILE *fp;
    //如果未找到切片则返回0
    if((fp = fopen(filename.toStdString().c_str(), "rb")) == NULL)
        data = toByteString(0);
    else
    {
        char c;
        int len = 0;
        string s;
        while(fread((char*)&c, sizeof(char), 1, fp) == 1)
        {
            s.push_back(c);
            ++len;
        }
        data = toByteString(len) + s;
        fclose(fp);
    }
    if(sendPacket(sockfd, cmd_getTile, data) != 0)
        return SEND_ERROR;

    return 0;
}

//传送地图标记
int Connection::getSymbol(string &data)
{
    int level = fromByteString<int>(data);
    int x = fromByteString<int>(data);
    int y = fromByteString<int>(data);
    int side = fromByteString<int>(data);

    data.clear();
    QRectF rect(x, y, side, side);
    QList<Point *> points = view->getVsibleSymbols(rect, level);
    data += toByteString(points.size());
    int len;
    string name;
    for(int i = 0; i < points.size(); ++i)
    {
        name = points[i]->getName().toStdString();
        len = name.size();
        data += toByteString(len) + name;
        data += toByteString(points[i]->getPoint().x()) + toByteString(points[i]->getPoint().y());
    }
    qDebug() << points.size();
    if(sendPacket(sockfd, cmd_getSymbol, data) != 0)
        return SEND_ERROR;

    return 0;
}


/*************************************
 *                           任务部分                                     *
 * ************************************/
//获取任务
int Connection::getTask(string &data)
{
    data.clear();

    if(taskState == Task_none || taskState == Task_run)
        data = toByteString(0);
    else
    {
        data = toByteString(1); //标识

        data += toByteString(path->truckId);    // 车辆编号
        // 配送点
        int cnt = path->places.size();
        data += toByteString(cnt);
        int len;
        string s;
        for(int i = 0; i < cnt; ++i)
        {
            // coordinate
            data += toByteString(path->places[i].coord.x()) + toByteString(path->places[i].coord.y());
            // title of place
            s = path->places[i].title.toStdString();
            len = s.size();
            data += toByteString(len) + s;
            // type of place
            data += toByteString((int)path->places[i].type);
        }
        // 货物清单
        cnt = path->cargos.size();
        for(int i = 0; i < cnt; ++i)
        {
            s = path->cargos[i].toStdString();
            len = s.size();
            data += toByteString(len) + s;
        }
    }
    if(sendPacket(sockfd, cmd_getTask, data) != 0)
        return SEND_ERROR;

    return 0;
}

//接受任务
int Connection::acptTask(string &data)
{
    qDebug() << "accept task";
    taskState = Task_run;
    emit sg_acptTask();
}

int Connection::posChange(string &data)
{
    /////////////////////////////TODO
    emit sg_posChange();
}

int Connection::taskFinish(string &data)
{
    taskState = Task_none;
    emit sg_taskFinish();
}

//任务失败
int Connection::taskFail(string &data)
{
    taskState = Task_none;
    emit sg_taskFail();
}


//提交路况
int Connection::submitRoad(string &data)
{
    int type, len;
    QString addr, detail;
    type =fromByteString<int>(data);
    len = fromByteString<int>(data);
    addr = QString::fromStdString(fromByteString(data, len));
    len = fromByteString<int>(data);
    detail = QString::fromStdString(fromByteString(data, len));

    emit sg_upload(acc.username, type, addr, detail);

    return 0;
}
