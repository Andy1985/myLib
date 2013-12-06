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

/*
static int count;

static void recvfrom_int(int signo)
{
	printf("\nreceived %d datagrams\n",count);
	exit(0);
}
*/


void dg_echo(int sockfd,struct sockaddr* pcliaddr,socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];
	char line[MAXLINE];

	//signal(SIGINT,recvfrom_int);

	for (;;)
	{
		len = clilen;
		n = recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len);
		printf("recvfrom from %s,port %d\n",
			inet_ntop(AF_INET,&(((struct sockaddr_in *)pcliaddr)->sin_addr),line,sizeof(line)),
			ntohs(((struct sockaddr_in *)pcliaddr)->sin_port));
		len = clilen;
		sendto(sockfd,mesg,n,0,pcliaddr,len);
	}
	
}

int main(int argc,char* argv[])
{
	int sockfd;

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;

	/* create sockfd */
	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	/* init inet addr */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);
	
	/* bind addr & port */
	bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	/* proceed client data*/
	dg_echo(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	/* close sockfd */
	close(sockfd);
	exit(0);
}
