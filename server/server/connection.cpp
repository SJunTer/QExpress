#include"packet.h"
#include "mapview.h"
#include "connection.h"
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
        case cmd_signIn:  //用户登录
            if(testUsrPwd(data) == LOGIN_YES)
            {
                emit signIn(id);
                qDebug() << "emit signal";
            }
            break;
        case cmd_signUp: break;     //用户注册
        case cmd_preLoad: preLoad(data); break;     //预加载地图信息
        case cmd_getTile: getTile(data); break;     //获取切片
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
