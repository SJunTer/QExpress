#ifndef SERVER_H
#define SERVER_H

#define PORTNO 6666
#define MAX_CONN 10

#include <QThread>
#include <QList>

#include "../network/socket.h"
#include "../network/commands.h"
#include "connection.h"

class MapView;

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(QObject *parent = 0);
    void stop();
    void setMap(MapView *m) { map = m; }

protected:
    void run();

private:
    volatile bool stopped;
    ServerSocket server;
    MapView *map;

    void addConn(int sockfd);

public slots:
    void close(int sockfd);
};

#endif // SERVER_H
