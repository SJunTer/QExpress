#include "packet.h"
#include "unistd.h"
#include "../security/des.h"
#include <iostream>

using namespace std;

/** 发送数据包 **/
int sendPacket(int sockfd, commandType cmd, const string &s_data)
{
//    cout << "send packet------------------" << endl;
//    cout << "send_size: " << s_data.size()<< endl;
    string wrapper = s_data;
    DES_Encrypt(wrapper);

    int totalLen, tempLen;
    totalLen = tempLen = wrapper.size();

    Packet packet;
    packet.dataLen = tempLen;
    packet.cmd = cmd;

    while(tempLen > 0)
    {
        if(tempLen > DATA_LEN) // 数据长度超出最大限度则分包
        {
            packet.flag = true;
            packet.packetLen = DATA_LEN;
            memcpy(packet.data, wrapper.c_str()+(totalLen-tempLen), DATA_LEN);
            /*			cout <<  packet.dataLen << " " << packet.flag << " " << packet.cmd
                << " " << packet.packetLen << endl;*/
            int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
 //           cout << "send length: " << sendLen << endl;
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
            memcpy(packet.data, wrapper.c_str()+(totalLen-tempLen), tempLen);
            /*		cout << packet.dataLen << " " << packet.flag << " " << packet.cmd
                << " " << packet.packetLen << endl;*/
            int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
//            cout << "send length: " << sendLen << endl;
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
// 确保接包正常
int myRead(int sock, void *buf, int len)
{
    int tempLen = 0;
    while(tempLen < len)
    {
        int readLen = read(sock, (char*)buf+tempLen, len-tempLen);
        if(readLen == -1)
            return -1;
        else if(readLen == 0)
        {
            if(tempLen == 0)
                return 0;
            else
                return tempLen;
        }
        else
            tempLen += readLen;
    }
    return tempLen;
}

/** 接收数据包 **/
int recvPacket(int sockfd, commandType *cmd, std::string &s_data)
{
//    cout << "receive packet-----------------------" << endl;
    Packet packet;
    s_data.clear();
    do
    {
        int readLen = myRead(sockfd, (char*)&packet, PACKET_LEN);
 /*       cout << "read length: " << readLen << endl;
                    cout << packet.dataLen << " " << packet.flag << " " << packet.cmd
            << " " << packet.packetLen << endl;*/
        if(readLen == -1)//读取错误
            return READ_ERROR;
        else if(readLen == 0)//返回0说明客户端已关闭连接
            return CLIENT_CLOSE;
        else if(readLen != PACKET_LEN)//包不完整
            return PACKET_LOSS;
        else
        {
            for(int n = 0; n < packet.packetLen; ++n)
                s_data.push_back(packet.data[n]);
        }
    }while(packet.flag);

    *cmd = packet.cmd;
    DES_Decrypt(s_data);

//    cout << "byte received: " << s_data.size() << endl;
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

