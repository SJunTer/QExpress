#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <string>
#include "accwidget.h"


 // 任务状态
enum TaskState {
    Task_none,  //当前无任务
    Task_wait,  //有任务未接受
    Task_run    //正在执行任务
};


QT_BEGIN_NAMESPACE
class MapView;
class DeliveryPath;
class Dbsql;
QT_END_NAMESPACE


class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(Dbsql *d, int s, MapView *v);

private:
    int sockfd;
    Account acc;
    DeliveryPath *path;
    TaskState taskState;
    MapView *view;
    Dbsql *dbsql;

    bool closed;    // 客户端关闭
    bool login;     // 是否登录


    //*----------------服务接口-----------------*//
    int signIn(std::string &data);
    int signUp(std::string &data);
    int getUserInfo(std::string &data);
    int changeUserInfo(std::string &data);
    int changePwd(std::string &data);

    int preLoad(std::string &data);
    int getTile(std::string &data);
    int getSymbol(std::string &data);

    int getTask(std::string &data);
    int acptTask(std::string &data);
    int posChange(std::string &data);
    int taskFinish(std::string &data);
    int taskFail(std::string &data);

    int submitRoad(std::string &data);

signals:
    // 客户端退出
    void taskFinished();
    void close(int);

    void sg_signIn(int i);
    void sg_signUp(Account a);
    void sg_signOut(int i);
    void sg_changeInfo(Account a);

    // Task related
    void sg_acptTask();
    void sg_posChange();
    void sg_taskFinish();
    void sg_taskFail();

    void sg_upload(QString usr, int type, QString addr, QString detail);

public slots:
    void start();
    void stop();

    void getTask(DeliveryPath *p);
};

#endif // CONNECTION_H
