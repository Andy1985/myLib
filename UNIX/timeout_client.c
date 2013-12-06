#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "unp.h"
#include "sum.h"

#define MAXLINE 1024
#define DAYTIME_PORT 5555

/*
static void sig_alarm(int signo)
{
	return;
}
*/

void dg_cli(FILE* fp,int sockfd,const struct sockaddr* pservaddr,socklen_t servlen)
{
	int n;
	//int i;
	char sendline[MAXLINE];
	char recvline[MAXLINE + 1];
	char buff[MAXLINE];

	struct timeval tv;

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));

	//struct sockaddr_in cliaddr;

	/*
	for (i = 0; i < 2000; ++i)
	{
		sendto(sockfd,sendline,MAXLINE,0,pservaddr,servlen);
	}
	*/

	socklen_t len;
	struct sockaddr* preply_addr;
	preply_addr = (struct sockaddr*)malloc(sizeof(struct sockaddr));

	/*
	connect(sockfd,(struct sockaddr*)pservaddr,servlen);

	socklen_t len = sizeof(cliaddr);
	getsockname(sockfd,(struct sockaddr*)&cliaddr,&len);
	
	printf("local address %s\n",inet_ntop(AF_INET,&(cliaddr.sin_addr),buff,sizeof(buff)));
	*/
	
	//signal(SIGALRM,sig_alarm);

	while (fgets(sendline,MAXLINE,fp) != NULL)
	{
		/*
		write(sockfd,sendline,strlen(sendline));
		n = read(sockfd,recvline,MAXLINE);
		*/
		
		sendto(sockfd,sendline,strlen(sendline),0,pservaddr,servlen);
		len = servlen;
		
		//alarm(5);
		//if (readable_timeo(sockfd,5) == 0)
		//{
		//	fprintf(stderr,"socket timeout\n");
		//}
		//else
		//{
		//	n = recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len);
		//}	

		n = recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len);

		if (n < 0)
		{
			if (errno == EWOULDBLOCK)
			{
				fprintf(stderr,"socket timeout\n");
				continue;
			}
			else
			{
				fprintf(stderr,"recvfrom error\n");
				exit(0);
			}
		}
		else
		{
			if (len != servlen || memcmp(pservaddr,preply_addr,len) != 0)
			{
				printf("reply from %s (ignored)\n",
					inet_ntop(AF_INET,
							&(((struct sockaddr_in *)preply_addr)->sin_addr),buff,sizeof(buff)));
				continue;
			}
			//alarm(0);
			recvline[n] = '\0';
			fputs(recvline,stdout);
		}
	}
}
	

int main(int argc,char* argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	if (argc != 2)
	{
		fprintf(stderr,"Usage: %s <IP address>\n",argv[0]);
		return -1;
	}

	/* create sockfd */
	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		fprintf(stderr,"socket error\n");
		exit(-1);
	}

	/* init inet addr struct */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DAYTIME_PORT);

	if (inet_pton(AF_INET,argv[1],&servaddr.sin_addr) <= 0)
	{
		fprintf(stderr,"inet_pton error for %s\n",argv[1]);
		return -2;
	}

	dg_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	/* close sockfd */
	close(sockfd);

	exit(0);
}
