#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "unp.h"

#define MAXLINE 1024
#define DAYTIME_PORT 13

int main(int argc,char* argv[])
{
	int sockfd;
	int n;
	char recvline[MAXLINE + 1];
	//struct sockaddr_in servaddr;

	if (argc != 3)
	{
		fprintf(stderr,"Usage: %s <IP address> <port>\n",argv[0]);
		return -1;
	}

	///* create sockfd */
	//if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	//{
	//	fprintf(stderr,"socket error\n");
	//	exit(-1);
	//}

	///* init inet addr struct */
	//bzero(&servaddr,sizeof(servaddr));
	//servaddr.sin_family = AF_INET;
	//servaddr.sin_port = htons(DAYTIME_PORT);

	//if (inet_pton(AF_INET,argv[1],&servaddr.sin_addr) <= 0)
	//{
	//	fprintf(stderr,"inet_pton error for %s\n",argv[1]);
	//	return -2;
	//}

	///* connect to server */
	//if (connect(sockfd,(struct sockaddr*)(&servaddr),sizeof(servaddr)) < 0)
	//{
	//	fprintf(stderr,"connect error\n");
	//	exit(-2);
	//}

	sockfd = tcp_connect(argv[1],argv[2]);

	/* read from server */
	while ((n = read(sockfd,recvline,MAXLINE)) > 0)
	{
		recvline[n] = '\0';
		/* print data to stdout */
		if (fputs(recvline,stdout) == EOF)
		{
			fprintf(stderr,"fputs error\n");
			exit(-3);
		}
	}

	if (n < 0)
	{
		fprintf(stderr,"read error\n");
		exit(-4);
	}

	/* close sockfd */
	close(sockfd);

	exit(0);
}
