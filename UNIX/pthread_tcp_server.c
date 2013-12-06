#include "unp.h"

void* doit(void*);

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

int main(int argc,char* argv[])
{
	int listenfd;
	socklen_t addrlen;
	socklen_t len;
	struct sockaddr* cliaddr;
	int* iptr;

	pthread_t tid;

	if (argc == 2)
	{
		listenfd = tcp_listen(NULL,argv[1],&addrlen);
	}
	else if (argc == 3)
	{
		listenfd = tcp_listen(argv[1],argv[2],&addrlen);
	}
	else
	{
		fprintf(stderr,"Usage: %s [<host>] <port#>\n",argv[0]);
		return -1;
	}

	cliaddr = (struct sockaddr*)malloc(sizeof(struct sockaddr));

	for (;;)
	{
		len = addrlen;
		iptr = (int*)malloc(sizeof(int));			
		sleep(1);
		*iptr = accept(listenfd,cliaddr,&len);
		pthread_create(&tid,NULL,&doit,(void*)iptr);
	}
}

void* doit(void* arg)
{
	int connfd = *((int*)arg);
	free(arg);

	pthread_detach(pthread_self());
	str_echo(connfd);
	close(connfd);

	return NULL;
}
