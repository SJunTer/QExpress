#include "packet.h"
#include "socket.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/wait.h>
#include <signal.h>

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

void response(int sockfd);
void sigCatcher(int n);

int main(int argc, char *argv[])
{

	if (argc < 2) 
	{
		cout << "ERROR, no port provided" << endl;
		exit(0);
	}
    int port = atoi(argv[1]);
    ServerSocket server;
	if((server.init(port)) != 0)
	{
        cout << "ERROR on init socket " << endl;
		exit(1);
	}
	server.startListen(5);

	// 信号处理
	// signal(SIGCHLD, SIG_IGN); lack fo portablity
	//	signal(SIGCHLD, sigCatcher);
	struct sigaction act;
	act.sa_handler = sigCatcher;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &act, NULL);

	int pid;
	while (1)
	{
        int res = server.acceptClient();
        if (res != 0)
		{
			cout << "ERROR on accept" << endl;
			exit(1);
		}
        pid = fork();
        if (pid < 0)  // 创建进程失败
		{
			cout << "ERROR on fork" << endl;
			exit(1);
		}
		if (pid == 0)	// 子进程
        {
            server.closeServer();
            response(server.client());
            server.closeClient();
			exit(0);
		}
		else	// 父进程
			continue;
    } //while
    server.closeServer();

    return 0; /* we never get here */
}

// 请求处理部分
void response (int sockfd)
{
	string s;
    commandType cmd;
	while(1)
    {
        int res = recvPacket(sockfd, &cmd, s);
        if(res == READ_ERROR)
        {
            cout << "ERROR on receive" << endl;
            break;
        }
        else if(res == CLIENT_CLOSE)
        {
            cout << "Client closed" << endl;
            break;
        }
        switch(cmd)
        {
        case cmd_default: break;
        case cmd_pic:
        {
            s.clear();
            FILE *fp;
            if((fp = fopen("/home/shen/info.png", "rb")) == NULL)
            {
                cout << "ERROR on open image file" << endl;
                break;
            }
            char c;
            int i = 0;
            while(fread((char*)&c, sizeof(char), 1, fp) != 0)
            {
                s.push_back(c);
                cout << i++ << endl;
            }
            sendPacket(sockfd, cmd, s);
            cout << "send successfully" << endl;
            break;
        }
        default: break;
        }
    }//while
}


// 处理zoombie
void sigCatcher(int n)
{
    int stat;
    waitpid(-1, &stat, WNOHANG);
}
