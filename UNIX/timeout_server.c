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

int max(int x,int y)
{
	return x > y ? x : y;
}

void str_echo(int connfd)
{
	//ssize_t n;
	char line[MAXLINE];

	FILE* fpin;
	FILE* fpout;

	fpin = fdopen(connfd,"r");
	fpout = fdopen(connfd,"w");
	
	for (;;)
	{
		if (fgets(line,MAXLINE,fpin) == NULL)
		{
			return;
		}
		fputs(line,fpout);
		/*
		if ((n = readline(connfd,line,MAXLINE)) == 0)
		{
			return;
		}
		writen(connfd,line,n);
		*/
	}
}

void sig_child(int signo)
{
	pid_t pid;
	int stat;
	
	while ((pid = waitpid(-1,&stat,WNOHANG)) > 0)
	{
		printf("child %d terminated\n",pid);
	}
	return;
}
int main(int argc,char* argv[])
{
	int listenfd;
	int connfd;
	int udpfd;
	int nready;
	int maxfdl;

	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;

	char mesg[MAXLINE];

	/* create sockfd */
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	/* init inet addr */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5555);
	
	/* set reuse */
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	
	/* bind addr & port */
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	/* listen */
	listen(listenfd,LISTENQ);	

	/* udp */
	udpfd = socket(AF_INET,SOCK_DGRAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5555);
	
	bind(udpfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	signal(SIGCHLD,sig_child);
	
	FD_ZERO(&rset);
	maxfdl = max(listenfd,udpfd) + 1;
	
	for (;;)
	{
		FD_SET(listenfd,&rset);
		FD_SET(udpfd,&rset);
		
		if ((nready = select(maxfdl,&rset,NULL,NULL,NULL)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				fprintf(stderr,"select error\n");
				exit(-1);
			}
		}

		if (FD_ISSET(listenfd,&rset))
		{
			len = sizeof(cliaddr);
			connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len);
			if ((childpid = fork()) == 0)
			{
				close(listenfd);
				str_echo(connfd);
				exit(0);
			}
			close(connfd);
		}

		if (FD_ISSET(udpfd,&rset))
		{
			len = sizeof(cliaddr);
			n = recvfrom(udpfd,mesg,MAXLINE,0,(struct sockaddr*)&cliaddr,&len);
			sendto(udpfd,mesg,n,0,(struct sockaddr*)&cliaddr,len);
		}
	}
	
	/* close sockfd */
	close(listenfd);
	exit(0);
}
