#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "unp.h"
#include "sum.h"

#include <signal.h>

#define MAXLINE 1024
#define DAYTIME_PORT 13

static void recvfrom_alarm(int signo)
{
	return;
}

void dg_cli(FILE* fp,int sockfd,const struct sockaddr* pservaddr,socklen_t servlen)
{
	int n;
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	char buff[MAXLINE];

	sigset_t sigset_alarm;
	sigset_t sigset_empty;

	fd_set rset;

	socklen_t len;
	struct sockaddr* preply_addr;
	preply_addr = (struct sockaddr*)malloc(sizeof(struct sockaddr));

	const int on = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&on,sizeof(on));

	FD_ZERO(&rset);
	sigemptyset(&sigset_empty);
	sigemptyset(&sigset_alarm);
	sigaddset(&sigset_alarm,SIGALRM);

	//signal(SIGALRM,recvfrom_alarm);

	while (fgets(sendline,MAXLINE,fp) != NULL)
	{
		if (sendto(sockfd,sendline,strlen(sendline),0,pservaddr,servlen) < 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));			
			return;
		}
		len = servlen;
		n = recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len);
			recvline[n] = 0;
			printf("from %s: %s",
				inet_ntop(AF_INET,&(((struct sockaddr_in *)preply_addr)->sin_addr),buff,sizeof(buff)),
				recvline);

		//sigprocmask(SIG_BLOCK,&sigset_alarm,NULL);
		//alarm(1);

	/*
		for (;;)
		{
			FD_SET(sockfd,&rset);
			n = pselect(sockfd + 1,&rset,NULL,NULL,NULL,&sigset_empty);
			if (n < 0)
			{
				if (errno == EINTR)
				{
					break;
				}
				else
				{
					fprintf(stderr,"pselect error\n");
					exit(-1);
				}
			}
			else if (n != 1)
			{
				fprintf(stderr,"pselect error: returned %d\n",n);
				exit(-1);
			}
			
			len = servlen;
			n = recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len);
				recvline[n] = 0;
				printf("from %s: %s",
					inet_ntop(AF_INET,&(((struct sockaddr_in *)preply_addr)->sin_addr),buff,sizeof(buff)),
					recvline);
		}
		*/
	}
}
	

int main(int argc,char* argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		fprintf(stderr,"Usage: %s <IP address>\n",argv[0]);
		return -1;
	}

	/* create sockfd */
	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		fprintf(stderr,"socket error\n");
		exit(-1);
	}

	/* init inet addr struct */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DAYTIME_PORT);

	if (inet_pton(AF_INET,argv[1],&servaddr.sin_addr) <= 0)
	{
		fprintf(stderr,"inet_pton error for %s\n",argv[1]);
		return -2;
	}

	dg_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	/* close sockfd */
	close(sockfd);

	exit(0);
}
