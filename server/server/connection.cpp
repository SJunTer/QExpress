#include"../network/packet.h"
#include "mapviewer/mapview.h"
#include "mapviewer/maplayer.h"
#include "connection.h"
#include <QGraphicsItem>
#include <QList>
#include <cstdio>
#include <QDebug>

using namespace std;

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
            if(testUsrPwd(data) == LOGIN_YES)
                emit signIn(id);
            break;
        case cmd_signUp:    //用户注册
            if(recvReg(data) == REG_YES)
                emit signUp(acc);
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
        case cmd_submitRoad:    //提交路况
            submitRoad(data);
            break;
        default: break;
        }
    }
    if(!closed)//客户端断开连接
    {
        emit signOut(id);
        emit close(sockfd);
    }
    emit taskFinished();
}

void Connection::stop()
{
    emit close(sockfd);
    closed = true;
}

// 验证用户名密码
int Connection::testUsrPwd(string &data)
{
    int usr_len = fromByteString<int>(data);
    string usr = fromByteString(data, usr_len);
    int pwd_len = fromByteString<int>(data);
    string pwd = fromByteString(data, pwd_len);

    data.clear();
    int ret;
    if(usr == "shen" && pwd == "abcd")
        ret = LOGIN_YES;
    else
        ret = LOGIN_NO;
    data += toByteString(ret);

    if(sendPacket(sockfd, cmd_signIn, data) != 0)
        return SEND_ERROR;

    return ret;
}

//用户注册请求
int Connection::recvReg(string &data)
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

    data.clear();
    int ret = REG_YES;
    data += toByteString(ret);
    if(sendPacket(sockfd, cmd_signUp, data) != 0)
        return SEND_ERROR;

    return ret;
}

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
    //TODO
//    qDebug() << level << x << y << side;
    data = toByteString(0);
    if(sendPacket(sockfd, cmd_getSymbol, data) != 0)
        return SEND_ERROR;

    return 0;
}

//获取任务
int Connection::getTask(string &data)
{
    qDebug() << "get task";
    data.clear();
    //发送数据
    data += toByteString(2);
    string s("牛肉干");
    int len = s.size();
    data += toByteString(len) + s;
    s = "Lenovo";
    len = s.size();
    data += toByteString(len) + s;
    data += toByteString(173);
    if(sendPacket(sockfd, cmd_getTask, data) != 0)
        return SEND_ERROR;

    return 0;
}

//接受任务
int Connection::acptTask(string &data)
{
    qDebug() << "accept task";
    data.clear();
    //TODO
}

//提交路况
int Connection::submitRoad(string &data)
{
    int type, len;
    string s;
    type =fromByteString<int>(data);
    len = fromByteString<int>(data);
    s =  fromByteString(data, len);
    len = fromByteString<int>(data);
    s = fromByteString(data, len);
    return 0;
}
