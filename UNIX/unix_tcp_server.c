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
#define UNIXSTR_PATH "/tmp/lixm"

int main(int argc,char* argv[])
{
	int listenfd;
	int connfd;
	int childpid;
	socklen_t clilen;
	struct sockaddr_un cliaddr;
	struct sockaddr_un servaddr;

	listenfd = socket(AF_LOCAL,SOCK_STREAM,0);
	unlink(UNIXSTR_PATH);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,UNIXSTR_PATH);

	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	listen(listenfd,LISTENQ);
	
	signal(SIGCHLD,sig_child);

	for (;;)
	{
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen)) < 0)
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

		if ((childpid = fork()) == 0)
		{
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}

		close(connfd);
	}

	close(listenfd);
	exit(0);
}
