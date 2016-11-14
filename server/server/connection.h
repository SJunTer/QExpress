#ifndef CONNECTION_H
#define CONNECTION_H

#include <QThread>
#include <string>

class MapView;

class Connection : public QThread
{
    Q_OBJECT
public:
    Connection(int sockfd, MapView *m);
    void stop();

protected:
    void run();

private:
    volatile bool stopped;
    int sockfd;
    MapView *map;

    int testUsrPwd(std::string &data);
    int preLoad(std::string &data);
    int getTile(std::string &data);

signals:
    void close(int);
};

#endif // CONNECTION_H
