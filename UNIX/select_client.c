#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "unp.h"
#include "sum.h"

#define MAXLINE 1024
#define DAYTIME_PORT 5555

int max(int x,int y)
{
	return (x > y) ? x : y;
}

void str_cli(FILE* fp,int sockfd)
{
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	int maxfdl;
	int stdineof;
	fd_set rset;

	stdineof = 0;
	FD_ZERO(&rset);

	for (;;)
	{
		if (stdineof == 0)
		{
			FD_SET(fileno(fp),&rset);
		}

		FD_SET(sockfd,&rset);
		maxfdl = max(fileno(fp),sockfd) + 1;

		if (select(maxfdl,&rset,NULL,NULL,NULL) < 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
		
		if (FD_ISSET(sockfd,&rset))
		{
			if (readline(sockfd,recvline,MAXLINE) == 0)
			{
				if (stdineof == 1)
				{
					return;
				}
				else
				{
					fprintf(stderr,"str_cli: server terminated prematurely\n");
					return;
				}
			}
			fputs(recvline,stdout);
		}
			
		if (FD_ISSET(fileno(fp),&rset))
		{
			if (fgets(sendline,MAXLINE,fp) == NULL)
			{
				stdineof = 1;
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			
			writen(sockfd,sendline,strlen(sendline));
		}
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
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
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

	/* connect to server */
	if (connect(sockfd,(struct sockaddr*)(&servaddr),sizeof(servaddr)) < 0)
	{
		fprintf(stderr,"connect error\n");
		exit(-2);
	}

	/* SO_LINGER */
	/*
	struct linger li = {1,0};
	if (setsockopt(sockfd,SOL_SOCKET,SO_LINGER,&li,sizeof(li)) < 0)
	{
		fprintf(stderr,"setsockopt linger error\n");
		exit(-3);
	}
	*/

	str_cli(stdin,sockfd);

	/* close sockfd */
	close(sockfd);

	exit(0);
}
