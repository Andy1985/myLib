#include "unp.h"

//void sig_urg(int);

int main(int argc,char* argv[])
{
	int listenfd;
	int connfd;
	int n;
	int justreadoob = 0;
	char buff[100];
	fd_set rset;
	fd_set xset;
	
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

	connfd = accept(listenfd,NULL,NULL);
	
	sleep(5);
	FD_ZERO(&rset);
	FD_ZERO(&xset);
	
	//signal(SIGURG,sig_urg);
	//fcntl(connfd,F_SETOWN,getpid());
	
	for (;;)
	{
		FD_SET(connfd,&rset);
		//if (justreadoob == 0)
		//{
			FD_SET(connfd,&xset);
		//}

		select(connfd + 1,&rset,NULL,&xset,NULL);

		if (FD_ISSET(connfd,&xset))
		{
			n = recv(connfd,buff,sizeof(buff) - 1,MSG_OOB);
			buff[n] = 0;
			printf("read %d OOB bytes: %s\n",n,buff);
			//justreadoob = 1;
			//FD_CLR(connfd,&xset);
		}
		
		if (FD_ISSET(connfd,&rset))
		{
			if ((n = read(connfd,buff,sizeof(buff) - 1)) == 0)
			{
				printf("received EOF\n");
				exit(0);
			}
			buff[n] = 0;
			printf("read %d bytes: %s\n",n,buff);
			//justreadoob = 0;
		}

		/*
		if ((n = read(connfd,buff,sizeof(buff) - 1)) == 0)
		{
			printf("received EOF\n");
			exit(0);
		}

		buff[n] = 0;
		printf("read %d bytes: %s\n",n,buff);
		*/
	}
	exit(0);
}

/*
void sig_urg(int signo)
{
	int n;
	char buff[100];
	printf("SIGURG received\n");
	n = recv(connfd,buff,sizeof(buff) - 1,MSG_OOB);
	buff[n] = 0;
	printf("read %d OOB byte: %s\n",n,buff);
}
*/
