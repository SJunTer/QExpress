/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>


#include "Packet.h"


void dostuff(int); /* function prototype */
void sigCatcher(int n);
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct sigaction act;

    if (argc < 2) {
	fprintf(stderr,"ERROR, no port provided\n");
	exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
	error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    // signal(SIGCHLD, SIG_IGN); lack fo portablity
    //	signal(SIGCHLD, sigCatcher);
    act.sa_handler = sigCatcher;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);
    while (1) {
	newsockfd = accept(sockfd,
		(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
	    error("ERROR on accept");
	pid = fork();
	if (pid < 0)
	    error("ERROR on fork");
	if (pid == 0)  {
	    close(sockfd);
	    printf("connected\n");
	    dostuff(newsockfd);
	    exit(0);
	}
	else close(newsockfd);
    } /* end of while */
    close(sockfd);
    return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
	char *data;
	commandType cmd;
	int len;
	recvPacket(sock, &cmd, &data, &len);
	printf("%u %d %s\n", cmd, len, data);
	switch(cmd)
	{
	case cmd_plus:
		printf("PLUS");  break;
	case cmd_multiply: 
		printf("MULTIPLY"); break;
	default: break;
	}
	if(data)
		free(data);
}

void sigCatcher(int n)
{
    int stat;
    waitpid(-1, &stat, WNOHANG);
}
