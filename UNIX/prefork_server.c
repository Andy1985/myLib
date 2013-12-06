#include "unp.h"
#include <stdlib.h>

static int nchildren;
static pid_t* pids;

int main(int argc,char* argv[])
{
	int listenfd;
	int i;
	socklen_t addrlen;
	void sig_int(int);
	pid_t child_make(int,int,int);

	if (argc == 3)
	{
		listenfd = tcp_listen(NULL,argv[1],&addrlen);
	}
	else if (argc == 4)
	{
		listenfd = tcp_listen(argv[1],argv[2],&addrlen);
	}
	else
	{
		fprintf(stderr,"Usage: %s [<host>] <port#> <#children>\n",argv[0]);
		return -1;
	}

	nchildren = atoi(argv[argc - 1]);
	pids = calloc(nchildren,sizeof(pid_t));
	my_lock_init();
	for (i = 0; i < nchildren; i++)
	{
		pids[i] = child_make(i,listenfd,addrlen);
	}
	
	signal(SIGINT,sig_int);
	for (;;)
	{
		pause();
	}

	exit(0);
}

void sig_int(int signo)
{
	int i;
	void pr_cpu_time(void);
	
	for (i = 0; i < nchildren; i++)
	{
		kill(pids[i],SIGTERM);
	}

	while (wait(NULL) > 0)
	{
		;
	}

	if (errno != ECHILD)
	{
		fprintf(stderr,"wait error\n");
		exit(-1);
	}

	pr_cpu_time();

	exit(0);
}

pid_t child_make(int i,int listenfd,int addrlen)
{
	pid_t pid;
	void child_main(int,int,int);
	
	if ((pid = fork()) == 0)
	{
		child_main(i,listenfd,addrlen);
	}

	return pid;
}

void child_main(int i,int listenfd,int addrlen)
{
	int connfd;
	void web_child(int);
	socklen_t clilen;
	struct sockaddr* cliaddr;

	cliaddr = (struct sockaddr*)malloc(addrlen);
	printf("child %ld starting\n",(long)getpid());
	
	for (;;)
	{
		clilen = addrlen;
		my_lock_wait();
		connfd = accept(listenfd,cliaddr,&clilen);
		my_lock_release();
		web_child(connfd);
		close(connfd);
	}
}
