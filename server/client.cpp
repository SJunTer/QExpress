#include "packet.h"
#include "socket.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <iostream>


using namespace std;


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	if (argc < 3) 
	{
		cout << "usage " << argv[0] << " hostname port" << endl;
		exit(0);
    }
    int portno = atoi(argv[2]);
	ClientSocket client;
    if(client.init(portno, argv[1]) != 0)
	{
		cout << "ERROR on init" << endl;
		exit(1);
	}
	if(client.connectTo() != 0)
	{
		cout << "ERROR on connect" << endl;
		exit(1);
	}

	string s;
    commandType cmd;

    s = "0";
    sendPacket(client.sock(), cmd_pic, s);
    recvPacket(client.sock(), &cmd, s);

    FILE *fp;
    if((fp = fopen("info.png", "wb")) == NULL)
    {
        cout << "open error" << endl;
        return -1;
    }
    for(int n = 0; n < s.size(); ++n)
    {
        char c = s[n];
        fwrite((char*)&c, sizeof(char), 1, fp);
    }
    cout << "write complete" << endl;


	client.closeSock();

	return 0;
}



