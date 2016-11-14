#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// 错误代码
#define SOCK_ERROR 0
#define BIND_ERROR 1
#define ACPT_ERROR 2
#define HOST_ERROR 3
#define CONN_ERROR 4


class ServerSocket
{
public:
    ServerSocket();

    int init(int port);
    void startListen(int connCnt);
    int acceptClient();
    void closeServer();
    void closeClient(int sockfd);
    int client() const;

private:
    int serv_sockfd;    // 服务器sock
    int cli_sockfd;     // 客户端sock
    struct sockaddr_in serv_addr;    // 服务器地址
    struct sockaddr_in cli_addr;    // 客户端地址
};


class ClientSocket
{
public:
    ClientSocket();

    int init(int port, const char *hostname);
    void setTimeout(int sec) { timeout.tv_sec = sec; }
    int connectTo();
    void closeSock();
    int sock() const;

private:
    int sockfd;
    struct timeval timeout;
    struct sockaddr_in addr;
};



#endif
