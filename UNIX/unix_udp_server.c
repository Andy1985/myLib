#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#include "unp.h"
#include "sum.h"

#define MAXLINE 1024

#define UNIXSTR_PATH "/tmp/lixm"
void dg_echo(int sockfd,struct sockaddr* pcliaddr,socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];
	char line[MAXLINE];

	for (;;)
	{
		len = clilen;
		n = recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len);
			printf("recvfrom from %s,port %d\n",
				inet_ntop(AF_INET,&(((struct sockaddr_in *)pcliaddr)->sin_addr),line,sizeof(line)),
				ntohs(((struct sockaddr_in *)pcliaddr)->sin_port));
		sendto(sockfd,mesg,n,0,pcliaddr,len);
	}
}

int main(int argc,char* argv[])
{
	int sockfd;
	struct sockaddr_un servaddr;
	struct sockaddr_un cliaddr;

	sockfd = socket(AF_LOCAL,SOCK_DGRAM,0);
	unlink(UNIXSTR_PATH);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,UNIXSTR_PATH);

	bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	dg_echo(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	close(sockfd);

	exit(0);
}
