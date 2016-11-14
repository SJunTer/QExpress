#include "packet.h"
#include "unistd.h"

//#include <iostream>

using namespace std;

/** 发送数据包 **/
int sendPacket(int sockfd, commandType cmd, const string &s_data)
{
//    cout << "send packet:" << endl;
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
            /*			cout <<  packet.dataLen << " " << packet.flag << " " << packet.cmd
                << " " << packet.packetLen << endl;*/
            int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
//            cout << sendLen << endl;
            if(sendLen == -1)
                return WRITE_ERROR;
            else if(sendLen != PACKET_LEN) // 丢包
                return PACKET_LOSS;
            else
                tempLen -= DATA_LEN;
        }
        else
        {
            packet.flag = false;
            packet.packetLen = tempLen;
            memcpy(packet.data, s_data.c_str()+(totalLen-tempLen), tempLen);
            /*		cout << packet.dataLen << " " << packet.flag << " " << packet.cmd
                << " " << packet.packetLen << endl;*/
            int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
//            cout << sendLen << endl;
            if(sendLen == -1)
                return WRITE_ERROR;
            else if(sendLen != PACKET_LEN)
                return PACKET_LOSS;
            else
                tempLen = 0;
        }
    }//while

    return 0;
}

/** 接收数据包 **/
int recvPacket(int sockfd, commandType *cmd, std::string &s_data)
{
//    cout << "receive packet:" << endl;
    Packet packet;
    s_data.clear();
    do
    {
        int readLen = read(sockfd, (char*)&packet, PACKET_LEN);
//        cout << readLen << endl;
        /*			cout << packet.dataLen << " " << packet.flag << " " << packet.cmd
            << " " << packet.packetLen << endl;*/
        if(readLen == -1)//读取错误
            return READ_ERROR;
        else if(readLen == 0)//返回0说明客户端已关闭套接字
            return CLIENT_CLOSE;
        else if(readLen != PACKET_LEN)//包不完整
            return PACKET_LOSS;
        else
        {
            for(int n = 0; n < packet.packetLen; ++n)
                s_data.push_back(packet.data[n]);
        }
    }while(packet.flag);
//    cout << s_data.size() << endl;

    *cmd = packet.cmd;

    return 0;
}


std::string fromByteString(std::string &s, int len)
{
    std::string str;
    for(int index = 0; index < len; ++index)
    {
        str.push_back(s[index]);
    }
    s = s.substr(len, s.size()-1);
    return str;
}

