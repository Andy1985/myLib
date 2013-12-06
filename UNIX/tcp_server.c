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
	int nready;

	ssize_t n;
	char line[MAXLINE];
	
	struct pollfd client[OPEN_MAX];

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t clilen;

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

	/* set pollfd */
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i = 1; i < OPEN_MAX; i++)
	{
		client[i].fd = -1;
	}

	maxi = 0;

	for (;;)
	{
		nready = poll(client,maxi + 1,INFTIM);
		if (client[0].revents & POLLRDNORM)
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
			
			printf("connection from %s,port %d\n",
				inet_ntop(AF_INET,&cliaddr.sin_addr,line,sizeof(line)),
				ntohs(cliaddr.sin_port));

			for (i = 1; i < OPEN_MAX; i++)
			{
				if (client[i].fd < 0)
				{
					client[i].fd = connfd;
					break;
				}
			}

			if (i == OPEN_MAX)
			{
				fprintf(stderr,"too many clients\n");
				return -1;
			}
			
			client[i].events = POLLRDNORM;
			if (i > maxi)
			{
				maxi = i;
			}
				
			if (--nready <= 0)
			{
				continue;
			}
		}
		
		for (i = 1; i <= maxi; i++)
		{
			if ((sockfd = client[i].fd) < 0)
			{
				continue;
			}

			if (client[i].revents & (POLLRDNORM | POLLERR))
			{
				if ((n = readline(sockfd,line,MAXLINE)) < 0)
				{
					if (errno == ECONNRESET)
					{
						close(sockfd);
						client[i].fd = -1;
					}
					else
					{
						fprintf(stderr,"readline error\n");
						return -2;
					}
				}
				else if (n == 0)
				{
					close(sockfd);
					client[i].fd = -1;
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
