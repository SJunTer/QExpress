#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <string>

class MapView;

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(int s, MapView *v) : id(0), sockfd(s),
        view(v), closed(false) {}

private:
    int id; // 账户ID
    int sockfd;
    MapView *view;
    bool closed;

    //*----------------服务接口-----------------*//
    int testUsrPwd(std::string &data);
    int preLoad(std::string &data);
    int getTile(std::string &data);

signals:
    void close(int);
    void taskFinished();
    void signIn(int i);
    void signOut(int i);

public slots:
    void start();
    void stop();
};

#endif // CONNECTION_H
