#ifndef PACKET_H
#define PACKET_H


#define PACKET_LEN (sizeof(Packet))
#define DATA_LEN 1024

#include <string>
#include <cstdlib>
#include <cstring>
#include "commands.h"

// 错误代码
#define PACKET_LOSS 0
#define SEND_ERROR 1
#define READ_ERROR 2
#define CLIENT_CLOSE 3



struct Packet
{
	int dataLen;		// 传输数据总长度
	bool flag;			// 是否分包
	commandType cmd;	// 命令代码
	int packetLen;		// 包内数据长度
	char data[DATA_LEN];	// 数据段
};

// 发送接受包
int sendPacket(int sockfd, commandType cmd, const std::string &s_data);
int recvPacket(int sockfd, commandType *cmd, std::string &s_data);

// 数据转化为字符串
template<typename T>
std::string toByteString(T n)
{
    std::string s;
    char temp[sizeof(T)];
    memcpy(temp, &n, sizeof(T));
    for(int i = 0; i < sizeof(T); ++i)
    {
        s.push_back(temp[i]);
    }
    return s;
}

// 将字符串转化为数据
template<typename T>
T fromByteString(std::string &s)
{
    T n;
    char temp[sizeof(T)];
    for(int index = 0; index < sizeof(T); ++index)
    {
        temp[index] = s[index];
    }
    s = s.substr(sizeof(T), s.size()-1);
    memcpy(&n, temp, sizeof(T));
    return n;
}

#endif
