#include "unp.h"
#include <stdlib.h>

typedef struct
{
	pid_t child_pid;
	int child_pipefd;
	int child_status;
	long child_count;
} Child;

Child* cptr;

static int nchildren;

int max(int x,int y)
{
	return x > y ? x : y;
}

int main(int argc,char* argv[])
{
	int listenfd;
	int i;
	int navail;
	int maxfd;
	int nset;
	int connfd;
	int rc;
	socklen_t addrlen;
	socklen_t clilen;
	ssize_t n;
	fd_set rset;
	fd_set masterset;
	
	struct sockaddr* cliaddr;

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

	FD_ZERO(&masterset);
	FD_SET(listenfd,&masterset);
	maxfd = listenfd;
	cliaddr = (struct sockaddr*)malloc(addrlen);

	nchildren = atoi(argv[argc - 1]);
	navail = nchildren;
	cptr = (Child*)calloc(nchildren,sizeof(Child));

	for (i = 0; i < nchildren; i++)
	{
		child_make(i,listenfd,addrlen);
		FD_SET(cptr[i].child_pipefd,&masterset);
		maxfd = max(maxfd,cptr[i].child_pipefd);
	}
	
	signal(SIGINT,sig_int);
	for (;;)
	{
		rset = masterset;
		if (navail <= 0 )
		{
			FD_CLR(listenfd,&rset);
		}

		nset = select(maxfd + 1,&rset,NULL,NULL,NULL);
		
		if (FD_ISSET(listenfd,&rset))
		{
			clilen = addrlen;
			connfd = accept(listenfd,cliaddr,&clilen);
			for (i = 0; i < nchildren; i++)
			{
				if (cptr[i].child_status == 0)
				{
					break;
				}
			}
			
			if (i == nchildren)
			{
				fprintf(stderr,"no available children\n");
				return -1;
			}
			
			cptr[i].child_status = 1;
			cptr[i].child_count++;
			navail--;

			n = write_fd(cptr[i].child_pipefd,"",1,connfd);
			close(connfd);
			if (--nset == 0)
			{
				continue;
			}
		}

		for (i = 0; i < nchildren; i++)
		{
			if (FD_ISSET(cptr[i].child_pipefd,&rset))
			{
				if ((n = read(cptr[i].child_pipefd,&rc,1)) == 0)
				{
					fprintf(stderr,"child %d terminateed unexpectedly\n",i);
				}

				cptr[i].child_status = 0;
				navail++;

				if (--nset == 0)
				{
					break;
				}
			}
		}
	}

	exit(0);
}

void sig_int(int signo)
{
	int i;
	void pr_cpu_time(void);
	
	for (i = 0; i < nchildren; i++)
	{
		kill(cptr[i].child_pid,SIGTERM);
		printf("%d: %ld\n",i,cptr[i].child_count);
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
	int sockfd[2];
	pid_t pid;
	void child_main(int,int,int);

	socketpair(AF_LOCAL,SOCK_STREAM,0,sockfd);
	
	if ((pid = fork()) == 0)
	{
		dup2(sockfd[1],STDERR_FILENO);
		close(sockfd[0]);
		close(sockfd[1]);
		close(listenfd);
		child_main(i,listenfd,addrlen);
	}

	close(sockfd[1]);
	cptr[i].child_pid = pid;
	cptr[i].child_pipefd = sockfd[0];
	cptr[i].child_status = 0;

	return pid;
}

void child_main(int i,int listenfd,int addrlen)
{
	char c;
	int connfd;
	ssize_t n;
	void web_child(int);

	printf("child %ld starting\n",(long)getpid());

	for (;;)
	{
		if ((n = read_fd(STDERR_FILENO,&c,1,&connfd)) == 0)
		{
			fprintf(stderr,"read_fd returned 0\n");
			return; 
		}
		if (connfd < 0)
		{
			fprintf(stderr,"no descriptor from read_fd\n");
			return;
		}

		web_child(connfd);
		close(connfd);
		write(STDERR_FILENO,"",1);
	}
}
