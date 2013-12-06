#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define MAXLINE 1024

#include "unp.h"

int main(int argc,char* argv[])
{
	int listenfd;
	int connfd;
	//struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t len;
	socklen_t addrlen;
	char buff[MAXLINE];
	time_t ticks;

	daemon_init(argv[0],LOG_LOCAL6);

	if (argc != 3)
	{
		syslog(LOG_ERR,"Usage: %s <ip> <port>\n",argv[0]);
		exit(-1);
	}
	
	listenfd = tcp_listen(argv[1],argv[2],&addrlen);
	///* create listenfd */
	//listenfd = socket(AF_INET,SOCK_STREAM,0);

	///* init inet addr */
	//bzero(&servaddr,sizeof(servaddr));
	//servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//servaddr.sin_port = htons(13);
	//
	///* bind addr & port */
	//bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	//
	///* set listening queue */
	//listen(listenfd,LISTENQ);

	for (;;)
	{
		/* blocked & waiting for client to connect */
		len = sizeof(cliaddr);
		connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len);

		syslog(LOG_INFO,"connection from %s,port %d\n",
			inet_ntop(AF_INET,&cliaddr.sin_addr,buff,sizeof(buff)),
			ntohs(cliaddr.sin_port));

		ticks = time(NULL);		
		snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));

		/* write data to client */
		write(connfd,buff,strlen(buff));

		/* close client link */
		close(connfd);
	}
	
	/* close listenfd */
	close(listenfd);
	exit(0);
}
