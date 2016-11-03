#include "Packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sendPacket(int sockfd, commandType cmd, char *data, int len)
{
	int tempLen = len;
	Packet packet;

	packet.dataLen = len;
	packet.cmd = cmd;
	
	while(tempLen > 0)
	{
		if(tempLen > DATA_LEN) // 数据长度超出最大限度则分包
		{
			printf("分包\n");
			packet.flag = 1;
			packet.packetLen = DATA_LEN;
			memcpy(packet.data, data+(len-tempLen), DATA_LEN);
/**/		printf("%d %d %u %d %s\n", packet.dataLen, packet.flag, packet.cmd, packet.packetLen, packet.data);
//			int sendLen = send(sockfd,(char*)&packet,PACKET_LEN,0);
			int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
			if(sendLen == -1)
			{
				printf("send error\n");
				return -1;
			}
			else if(sendLen != PACKET_LEN)
			{
				printf("packet loss when sending\n");
				continue;
			}
			else
			{
				tempLen -= DATA_LEN;
				printf("next packet\n");
			}
		}
		else
		{
			printf("无分包\n");
			packet.flag = 0;
			packet.packetLen = tempLen;
			memcpy(packet.data, data+(len-tempLen), tempLen);
/**/		printf("%d %d %u %d %s\n", packet.dataLen, packet.flag, packet.cmd, packet.packetLen, packet.data);
			int sendLen = write(sockfd, (char*)&packet, PACKET_LEN);
			if(sendLen == -1)
			{
				printf("send error\n");
				return -1;
			}
			else if(sendLen != PACKET_LEN)
			{
				printf("packet loss when sending\n");
				continue;
			}
			else
				tempLen = 0;
		}
	}//while
	return 0;
}

int recvPacket(int sockfd, commandType *cmd, char **data, int *len)
{
	Packet packet;
	int tempLen = 0;
	int readLen = 0;

	do
	{
		readLen = read(sockfd, (char*)&packet, PACKET_LEN);
		if(readLen == -1)
			return -1;
		else if(readLen != PACKET_LEN)
			return -1;
		if(tempLen == 0)
		{
/**/		printf("%d %d %u %d %s\n", packet.dataLen, packet.flag, packet.cmd, packet.packetLen, packet.data);
			*data = (char *)malloc(packet.dataLen);
		}
		memcpy(*data+tempLen, packet.data, packet.packetLen);
		tempLen += packet.packetLen;
	}while(packet.flag);

	*cmd = packet.cmd;
	*len = packet.dataLen;

	return 0;
}

