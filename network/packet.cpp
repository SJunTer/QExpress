#include "packet.h"
#include "unistd.h"

#include <iostream>

using namespace std;

/** 发送数据包 **/
int sendPacket(int sockfd, commandType cmd, const string &s_data)
{
    int totalLen, tempLen;
    totalLen = tempLen = s_data.size();

    Packet packet;
    packet.dataLen = tempLen;
    packet.cmd = cmd;

    while(tempLen > 0)
    {
        if(tempLen > DATA_LEN) // 数据长度超出最大限度则分包
        {
            packet.flag = true;
            packet.packetLen = DATA_LEN;
            memcpy(packet.data, s_data.c_str()+(totalLen-tempLen), DATA_LEN);
            /*			cout << "send" <<  packet.dataLen << " " << packet.flag << " " << packet.cmd
                << " " << packet.packetLen << " " << packet.data << endl;*/
            int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
            if(sendLen == -1)
                return SEND_ERROR;
            else if(sendLen != PACKET_LEN) // 丢包即重发
                continue;
            else
                tempLen -= DATA_LEN;
        }
        else
        {
            packet.flag = false;
            packet.packetLen = tempLen;
            memcpy(packet.data, s_data.c_str()+(totalLen-tempLen), tempLen);
            /*			cout << "send: " << packet.dataLen << " " << packet.flag << " " << packet.cmd
                << " " << packet.packetLen << " " << packet.data << endl;*/
            int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
            if(sendLen == -1)
                return SEND_ERROR;
            else if(sendLen != PACKET_LEN)
                continue;
            else
                tempLen = 0;
        }
    }//while

    return 0;
}

/** 接收数据包 **/
int recvPacket(int sockfd, commandType *cmd, std::string &s_data)
{
    Packet packet;
    s_data.clear();
    do
    {
        int readLen = read(sockfd, (char*)&packet, PACKET_LEN);
        if(readLen == -1)//读取错误
            return READ_ERROR;
        else if(readLen == 0)//返回0说明客户端已关闭套接字
            return CLIENT_CLOSE;
        else if(readLen != PACKET_LEN)//包不完整
            continue;
        else
        {
            for(int n = 0; n < packet.packetLen; ++n)
                s_data.push_back(packet.data[n]);
        }
    }while(packet.flag);

    *cmd = packet.cmd;

    return 0;
}



