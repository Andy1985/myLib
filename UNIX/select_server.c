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

int main(int argc,char* argv[])
{
	int listenfd;
	int connfd;
	int sockfd;
	int i;
	int maxi;
	int maxfd;

	int nready;
	int client[FD_SETSIZE];
	ssize_t n;
	
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t clilen;

	char line[MAXLINE];

	fd_set rset;
	fd_set allset;
	
	/* create listenfd */
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	/* init inet addr */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5555);
	
	/* bind addr & port */
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	/* set listening queue */
	listen(listenfd,LISTENQ);

	/* init */
	maxfd = listenfd;
	maxi = -1;
	for (i = 0; i < FD_SETSIZE; i++)
	{
		client[i] = -1;
	}

	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	for (;;)
	{
		rset = allset;
		nready = select(maxfd + 1,&rset,NULL,NULL,NULL);

		if (FD_ISSET(listenfd,&rset))
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
			printf("connection from %s,port %d\n",
				inet_ntop(AF_INET,&cliaddr.sin_addr,line,sizeof(line)),
				ntohs(cliaddr.sin_port));

			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}

			if (i == FD_SETSIZE)
			{
				fprintf(stderr,"too many clients\n");
				return -1;
			}
			
			FD_SET(connfd,&allset);
			
			if (connfd > maxfd)
			{
				maxfd = connfd;
			}

			if (i > maxi)
			{
				maxi = i;
			}

			if (--nready <= 0)
			{
				continue;
			}
		}


		for (i = 0; i <= maxi; i++)
		{
			if ((sockfd = client[i]) < 0)
			{
				continue;
			}

			if (FD_ISSET(sockfd,&rset))
			{
				if ((n = readline(sockfd,line,MAXLINE)) == 0)
				{
					close(sockfd);
					FD_CLR(sockfd,&allset);
					client[i] = -1;
				}
				else
				{
					writen(sockfd,line,n);
				}
				
				if (--nready <= 0)
				{
					break;
				}
			}
		}
	}
	
	/* close listenfd */
	close(listenfd);
	exit(0);
}
