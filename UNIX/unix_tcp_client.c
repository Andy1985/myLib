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
#define UNIXSTR_PATH "/tmp/lixm"

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
	struct sockaddr_un servaddr;

	sockfd = socket(AF_LOCAL,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,UNIXSTR_PATH);
	
	connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));	

	str_cli(stdin,sockfd);
	
	close(sockfd);
	exit(0);
}
