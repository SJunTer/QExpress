#ifndef SERVER_H
#define SERVER_H

#define PORTNO 6666
#define MAX_CONN 10

#include <QObject>
#include "accwidget.h"
#include "../network/socket.h"


QT_BEGIN_NAMESPACE
class MapView;
QT_END_NAMESPACE


class TcpServer : public QObject
{
    Q_OBJECT
public:
    TcpServer(MapView *v) : view(v),running(false) {}

private:
    MapView *view;
    ServerSocket server;
    bool running;

    void newConn(int sockfd);

signals:
    void taskFinished();
    void closeClient();
    void signIn(int i);
    void signUp(Account &a);
    void signOut(int i);

public slots:
    void start();   // 开启服务器
    void stop();    // 终止服务器

    void close(int sockfd);

};

#endif // SERVER_H

