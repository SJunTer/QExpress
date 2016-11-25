#ifndef SERVER_H
#define SERVER_H

#define PORTNO 6666
#define MAX_CONN 10

#include <QObject>
#include "accwidget.h"
#include "../network/socket.h"
#include "deliverywidget.h"


QT_BEGIN_NAMESPACE
class MapView;
class DeliveryPath;
class Dbsql;
QT_END_NAMESPACE


class TcpServer : public QObject
{
    Q_OBJECT
public:
    TcpServer(ServerSocket *s,Dbsql *d, MapView *v);

private:
    Dbsql *dbsql;
    MapView *view;
    ServerSocket *server;
    bool running;

    void newConn(int sockfd);

signals:
    void taskFinished();
    void closeClient();
    void closeClient(int id);

    void sg_signIn(int i);
    void sg_signUp(Account a);
    void sg_signOut(int i);
    void sg_changeInfo(Account a);

    void sg_acptTask(int id);
    void sg_posChange(int id, int pos);
    void sg_taskFinish(int id);
    void sg_taskFail(int id);

    void sg_upload(QString usr, int type, QString addr, QString detail);

    void sendTask(DeliveryPath path);  // 发送任务到客户端线程

public slots:
    void start();   // 开启服务器
    void stop();    // 终止服务器

    void close(int sockfd);

};

#endif // SERVER_H

