#include "unp.h"
#include <stdlib.h>

#define MAXN 16384

#define MAXLINE 1024

int main(int argc,char* argv[])
{
	int i;
	int j;	
	int fd;
	int nchildren;
	int nloops;
	int nbytes;
	pid_t pid;
	ssize_t n;
	
	char request[MAXLINE];
	char reply[MAXN];

	if (argc != 6)
	{
		fprintf(stderr,"Usage: %s <hostname> <port> <#children>"
			" <#loops/child> <#bytes/request>\n",
			argv[0]);
		return -1;
	}
	

	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);

	snprintf(request,sizeof(request),"%d\n",nbytes);
	
	for (i = 0; i < nchildren; i++)
	{
		if ((pid = fork()) == 0)
		{
			for (j = 0; j < nloops; j++)
			{
				fd = tcp_connect(argv[1],argv[2]);
				write(fd,request,strlen(request));
				//read(fd,reply,MAXN);
				if ((n = readn(fd,reply,nbytes)) != nbytes)
				{
					fprintf(stderr,"server returned %d bytes\n",n);	
					return -1;
				}
				close(fd);
			}
			printf("child %d done\n",i);
			exit(0);
		}
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

	exit(0);
}
