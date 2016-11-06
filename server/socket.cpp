#include "socket.h"
#include <cstdlib>
#include <cstring>

using namespace std;


/***---------------------Server-------------------***/
// 构造函数
ServerSocket::ServerSocket()
: serv_sockfd(-1)
, cli_sockfd(-1)
{
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
}

ServerSocket::~ServerSocket()
{
    serv_sockfd = -1;
    cli_sockfd = -1;
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
}

// 初始化套结字和地址并绑定
int ServerSocket::init(int port)
{
    serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sockfd < 0)
        return SOCK_ERROR;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
	// host byte order to network byte order
    serv_addr.sin_port = htons(port);
	
    if(bind(serv_sockfd, (struct sockaddr*) &(serv_addr), sizeof(serv_addr)) < 0)
        return BIND_ERROR;

    return 0;
}

// 开始监听
void ServerSocket::startListen(int connCnt)
{
    listen(serv_sockfd, connCnt);
}

// 接受连接
int ServerSocket::acceptClient()
{
    socklen_t len = sizeof(cli_addr);
    cli_sockfd = accept(serv_sockfd, (struct sockaddr*) &cli_addr, &len);
    if(cli_sockfd < 0)
        return ACPT_ERROR;
    else
        return 0;
}

// 关闭服务器连接
void ServerSocket::closeServer()
{
    close(serv_sockfd);
}

// 关闭客户端连接
void ServerSocket::closeClient()
{
    close(cli_sockfd);
}

// 返回指定客户端套接字
int ServerSocket::client() const
{
    return cli_sockfd;
}



/***---------------------Client-------------------***/
// 构造函数
ClientSocket::ClientSocket()
: sockfd(-1)
{
    memset(&addr, 0, sizeof(addr));
}

ClientSocket::~ClientSocket()
{
    sockfd = -1;
    memset(&addr, 0, sizeof(addr));
}

// 初始化客户端套结字和目标服务器的地址
int ClientSocket::init(int port, const char *hostname)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return SOCK_ERROR;

    struct hostent *host = gethostbyname(hostname);
    if (host == NULL)
        return HOST_ERROR;

    addr.sin_family = AF_INET;
    bcopy((char *)host->h_addr,
            (char *)&addr.sin_addr.s_addr,
            host->h_length);
    addr.sin_port = htons(port);

	return 0;
}

// 连接到服务器
int ClientSocket::connectTo()
{
    if (connect(sockfd,(struct sockaddr*) &addr, sizeof(addr)) < 0)
        return CONN_ERROR;
    else
        return 0;
}

// 关闭套接字
void ClientSocket::closeSock()
{
    close(sockfd);
}

// 返回客户端套接字fd
int ClientSocket::sock() const
{
    return sockfd;
}
