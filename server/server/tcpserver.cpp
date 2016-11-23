#include "tcpserver.h"
#include "../network/commands.h"
#include "connection.h"
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
    // 客户端断开连接
    connect(conn, SIGNAL(close(int)), this, SLOT(close(int)));

    connect(conn, SIGNAL(sg_signIn(int)), this, SIGNAL(sg_signIn(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_signUp(Account)), this, SIGNAL(sg_signUp(Account)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_signOut(int)), this, SIGNAL(sg_signOut(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_changeInfo(Account)), this, SIGNAL(sg_changeInfo(Account)), Qt::DirectConnection);

    connect(conn, SIGNAL(sg_acptTask()), this, SIGNAL(sg_acptTask()), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_posChange()), this, SIGNAL(sg_posChange()), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_taskFinish()), this, SIGNAL(sg_taskFinish()), Qt::DirectConnection);
    connect(conn, SIGNAL(sg_taskFail()), this, SIGNAL(sg_taskFail()), Qt::DirectConnection);

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
