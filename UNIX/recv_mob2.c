#include "unp.h"
int listenfd;
int connfd;
void sig_urg(int);

int main(int argc,char* argv[])
{
	int size;

	if (argc == 2)
	{
		listenfd = tcp_listen(NULL,argv[1],NULL);
	}
	else if (argc == 3)
	{
		listenfd = tcp_listen(argv[1],argv[2],NULL);
	}
	else
	{
		fprintf(stderr,"Usage: %s [<host>] <port#>\n",argv[0]);
		return -1;
	}

	size = 4096;
	setsockopt(listenfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));
	
	connfd = accept(listenfd,NULL,NULL);
	
	signal(SIGURG,sig_urg);
	fcntl(connfd,F_SETOWN,getpid());

	for (;;)
	{
		pause();
	}

	exit(0);
}

void sig_urg(int signo)
{
	int n;
	char buff[2048];
	printf("SIGURG received\n");
	n = recv(connfd,buff,sizeof(buff) - 1,MSG_OOB);
	if (n < 0)
	{
		fprintf(stderr,"recv error:%s\n",strerror(errno));
	}
	buff[n] = 0;
	printf("read %d OOB byte\n",n);
}
