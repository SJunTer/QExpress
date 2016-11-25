#include "tcpserver.h"
#include "../network/commands.h"
#include "connection.h"
#include "deliverywidget.h"
#include <QThread>
#include <QDebug>

// 创建客户端连接
TcpServer::TcpServer(ServerSocket *s, Dbsql *d, MapView *v) :
    server(s),
    dbsql(d),
    view(v),
    running(false)
{
    qRegisterMetaType<Account>("Account");
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<DeliveryPath>("DeliveryPath");
}

void TcpServer::newConn(int sockfd)
{
    QThread *thread = new QThread;
    Connection *conn = new Connection(dbsql, sockfd, view);
    conn->moveToThread(thread);
    connect(thread, SIGNAL(started()), conn, SLOT(start()));
    connect(conn, SIGNAL(taskFinished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), conn, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    // 切断客户端连接
    connect(this, SIGNAL(closeClient()), conn, SLOT(stop()), Qt::DirectConnection);
    connect(this, SIGNAL(closeClient(int)), conn, SLOT(stop(int)), Qt::DirectConnection);
    connect(this, SIGNAL(sendTask(DeliveryPath)), conn, SLOT(getTask(DeliveryPath)), Qt::DirectConnection);
    // 客户端断开连接
    connect(conn, SIGNAL(close(int)), this, SLOT(close(int)), Qt::DirectConnection);

    connect(conn, SIGNAL(sg_signIn(int)), this, SIGNAL(sg_signIn(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_signUp(Account)), this, SIGNAL(sg_signUp(Account)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_signOut(int)), this, SIGNAL(sg_signOut(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_changeInfo(Account)), this, SIGNAL(sg_changeInfo(Account)), Qt::DirectConnection);

    connect(conn, SIGNAL(sg_acptTask(int)), this, SIGNAL(sg_acptTask(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_posChange(int,int)), this, SIGNAL(sg_posChange(int,int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_taskFinish(int)), this, SIGNAL(sg_taskFinish(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_taskFail(int)), this, SIGNAL(sg_taskFail(int)), Qt::DirectConnection);

    connect(conn, SIGNAL(sg_upload(QString,int,QString,QString)), this, SIGNAL(sg_upload(QString,int,QString,QString)), Qt::DirectConnection);

    thread->start(); // 开启服务器线程
}

// 开启服务器
void TcpServer::start()
{
    server->startListen(MAX_CONN);
    running = true;
    while(1)
    {
        if(server->acceptClient() != 0)
        {
            qDebug() << "ERROR on accept";
            break;
        }
        qDebug()<<"get new connection";
        newConn(server->client());
    }
    qDebug() << "close";
    //free all connections
    emit closeClient();
    if(running)
        server->closeServer();

    emit taskFinished();
}

// 终止服务器
void TcpServer::stop()
{
    server->closeServer();
    running = false;
}


void TcpServer::close(int sockfd)
{
    server->closeClient(sockfd);
}
