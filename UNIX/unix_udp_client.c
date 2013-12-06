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
#define UNIXSTR_PATH "/tmp/lixm"

void dg_cli(FILE* fp,int sockfd,const struct sockaddr* pservaddr,socklen_t servlen)
{
	int n;
	//int i;
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	char buff[MAXLINE];

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

	while (fgets(sendline,MAXLINE,fp) != NULL)
	{
		/*
		write(sockfd,sendline,strlen(sendline));
		n = read(sockfd,recvline,MAXLINE);
		*/
		
		sendto(sockfd,sendline,strlen(sendline),0,pservaddr,servlen);
		len = servlen;
		n = recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len);
		/*
		if (len != servlen || memcmp(pservaddr,preply_addr,len) != 0)
		{
			printf("reply from %s (ignored)\n",
				inet_ntop(AF_INET,&(((struct sockaddr_in *)preply_addr)->sin_addr),buff,sizeof(buff)));
			continue;
		}
		*/

		recvline[n] = '\0';
		fputs(recvline,stdout);
	}
}

int main(int argc,char* argv[])
{
	int sockfd;
	struct sockaddr_un servaddr;
	struct sockaddr_un cliaddr;

	sockfd = socket(AF_LOCAL,SOCK_DGRAM,0);

	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	strcpy(cliaddr.sun_path,tmpnam(NULL));
	
	bind(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,UNIXSTR_PATH);

	dg_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	close(sockfd);
	exit(0);
}
