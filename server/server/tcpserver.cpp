#include "tcpserver.h"
#include "commands.h"
#include "connection.h"
#include <QThread>
#include <QDebug>

// 创建客户端连接
void TcpServer::newConn(int sockfd)
{
    QThread *thread = new QThread;
    Connection *conn = new Connection(sockfd, view);
    conn->moveToThread(thread);
    connect(thread, SIGNAL(started()), conn, SLOT(start()));
    connect(conn, SIGNAL(taskFinished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), conn, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    // 切断客户端连接
    connect(this, SIGNAL(closeClient()), conn, SLOT(stop()), Qt::DirectConnection);
    // 客户端断开连接
    connect(conn, SIGNAL(close(int)), this, SLOT(close(int))); //close socket first
    connect(conn, SIGNAL(signIn(int)), this, SIGNAL(signIn(int)), Qt::DirectConnection);
    connect(conn, SIGNAL(signOut(int)), this, SIGNAL(signOut(int)), Qt::DirectConnection);
    thread->start(); // 开启服务器线程
}

// 开启服务器
void TcpServer::start()
{
    if(server.init(PORTNO) != 0)
    {
        qDebug() << "ERROR on init socket";
    }
    else
    {
        server.startListen(MAX_CONN);
        running = true;
        while(1)
        {
            qDebug()<<"wait for connection request";
            if(server.acceptClient() != 0)
            {
                qDebug() << "ERROR on accept";
                break;
            }
            qDebug()<<"get new connection";
            newConn(server.client());
        }
        qDebug() << "close";
        //free all connections
        emit closeClient();
        if(running)
            server.closeServer();
    }
    qDebug() << "finish";
    emit taskFinished();
}

// 终止服务器
void TcpServer::stop()
{
    server.closeServer();
    running = false;
}


void TcpServer::close(int sockfd)
{
    server.closeClient(sockfd);
}
