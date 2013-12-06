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

void str_echo(int connfd)
{
	ssize_t n;
	char line[MAXLINE];
	
	for (;;)
	{
		if ((n = readline(connfd,line,MAXLINE)) == 0)
		{
			return;
		}
		writen(connfd,line,n);
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
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t len;
	char buff[MAXLINE];
	pid_t pid;
	
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

	/* kill child process */
	signal(SIGCHLD,sig_child);

	for (;;)
	{
		/* blocked & waiting for client to connect */
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				fprintf(stderr,"accept error\n");
				exit(-1);
			}
		}

		printf("connection from %s,port %d\n",
			inet_ntop(AF_INET,&cliaddr.sin_addr,buff,sizeof(buff)),
			ntohs(cliaddr.sin_port));

		/* fork */
		if ((pid = fork()) == 0)	
		{
			/* close listenfd */
			close(listenfd);

			str_echo(connfd);

			/* close connfd */
			close(connfd);

			/* exit from child process */
			exit(0);
		}

		/* close client link */
		close(connfd);
	}
	
	/* close listenfd */
	close(listenfd);
	exit(0);
}
