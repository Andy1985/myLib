#include "unp.h"

void* copyto(void*);

static int sockfd;
static FILE* fp;

#define MAXLINE 1024

void str_cli(FILE* fp_arg,int sockfd_arg)
{
	char recvline[MAXLINE];
	pthread_t tid;

	sockfd = sockfd_arg;
	fp = fp_arg;

	pthread_create(&tid,NULL,copyto,NULL);
	
	while (readline(sockfd,recvline,MAXLINE) > 0)
	{
		fputs(recvline,stdout);
	}
}

void* copyto(void* arg)
{
	char sendline[MAXLINE];
	while (fgets(sendline,MAXLINE,fp) != NULL)
	{
		writen(sockfd,sendline,strlen(sendline));
	}

	shutdown(sockfd,SHUT_WR);

	return NULL;
}

int main(int argc,char* argv[])
{
	int sockfd_main;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		fprintf(stderr,"Usage: %s <IP address>\n",argv[0]);
		return -1;
	}

	/* create sockfd */
	if ((sockfd_main = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		fprintf(stderr,"socket error\n");
		exit(-1);
	}

	/* init inet addr struct */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5555);

	if (inet_pton(AF_INET,argv[1],&servaddr.sin_addr) <= 0)
	{
		fprintf(stderr,"inet_pton error for %s\n",argv[1]);
		return -2;
	}

	/* connect to server */
	if (connect(sockfd_main,(struct sockaddr*)(&servaddr),sizeof(servaddr)) < 0)
	{
		fprintf(stderr,"connect error: %s\n",strerror(errno));
		exit(-2);
	}

	/* SO_LINGER */
	/*
	struct linger li = {1,0};
	if (setsockopt(sockfd,SOL_SOCKET,SO_LINGER,&li,sizeof(li)) < 0)
	{
		fprintf(stderr,"setsockopt linger error\n");
		exit(-3);
	}
	*/

	str_cli(stdin,sockfd_main);

	/* close sockfd */
	close(sockfd_main);

	exit(0);
}
