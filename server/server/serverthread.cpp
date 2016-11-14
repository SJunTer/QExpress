#include "serverthread.h"

#include <QDebug>

ServerThread::ServerThread(QObject *parent)
    : QThread(parent)
    , stopped(false)
{
}

void ServerThread::stop()
{
    stopped = true;
}

void ServerThread::run()
{
    if(server.init(PORTNO) != 0)
    {
        qDebug() << "ERROR on init socket";
        stop();
        return;
    }
    server.startListen(MAX_CONN);
    qDebug() << "start listening";
    while(!stopped)
    {
        if(server.acceptClient() != 0)
        {
            qDebug() << "ERROR on accept";
            stop();
            return;
        }
        addConn(server.client());
    }
    server.closeServer();
    qDebug() << "close server";
}

void ServerThread::addConn(int sockfd)
{
    Connection *conn = new Connection(sockfd, map);
    connect(conn, SIGNAL(close(int)), this, SLOT(close(int)));
    conn->start();
}

void ServerThread::close(int sockfd)
{
    server.closeClient(sockfd);
}
