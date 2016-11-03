#ifndef PACKET_H
#define PACKET_H

#ifdef __cplusplus
	extern "C" {
#endif

#define PACKET_LEN 1024
#define DATA_LEN (1024-4*sizeof(int))


typedef enum commandType {
	cmd_plus, cmd_multiply
} commandType;


typedef struct packet
{
	int dataLen;		// 传输数据总长度
	int flag;			// 是否分包
	commandType cmd;	// 命令代码
	int packetLen;		// 包内数据长度
	char data[DATA_LEN];	// 数据段
} Packet;


int sendPacket(int sockfd, commandType cmd, char *data, int len);
int recvPacket(int sockfd, commandType *cmd, char **data, int *len);



#ifdef __cplusplus
	}
#endif


#endif
