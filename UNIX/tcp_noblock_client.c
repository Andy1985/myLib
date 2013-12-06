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

int max(int x,int y)
{
	return x > y ? x : y;
}

void str_cli(FILE* fp,int sockfd)
{
	int maxfdpl;
	int val;
	int stdineof;
	ssize_t n;
	ssize_t nwritten;
	fd_set rset;
	fd_set wset;
	char to[MAXLINE];
	char fr[MAXLINE];
	char* toiptr;
	char* tooptr;
	char* friptr;
	char* froptr;
	
	val = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,val|O_NONBLOCK);
	
	val = fcntl(STDIN_FILENO,F_GETFL,0);
	fcntl(STDIN_FILENO,F_SETFL,val|O_NONBLOCK);

	val = fcntl(STDOUT_FILENO,F_GETFL,0);
	fcntl(STDOUT_FILENO,F_SETFL,val|O_NONBLOCK);

	toiptr = tooptr = to;
	friptr = froptr = fr;
	stdineof = 0;

	maxfdpl = max(max(STDIN_FILENO,STDOUT_FILENO),sockfd) + 1;
	
	for (;;)
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);

		/* read from stdin */
		if (stdineof == 0 && toiptr < &to[MAXLINE])
		{
			FD_SET(STDIN_FILENO,&rset);
		}

		/* read from socket */
		if (friptr < &fr[MAXLINE])
		{
			FD_SET(sockfd,&rset);
		}

		/* data to write to socket */
		if (tooptr != toiptr)
		{
			FD_SET(sockfd,&wset);
		}

		/* data to write to stdout */
		if (froptr != friptr)
		{
			FD_SET(STDOUT_FILENO,&wset);
		}

		select(maxfdpl,&rset,&wset,NULL,NULL);

		if (FD_ISSET(STDIN_FILENO,&rset))
		{
			if ((n = read(STDIN_FILENO,toiptr,&to[MAXLINE] - toiptr)) < 0)
			{
				if (errno != EWOULDBLOCK)
				{
					fprintf(stderr,"read error on stdin\n");
					exit(-1);
				}
			}
			else if (n == 0)
			{
				fprintf(stderr,"%s: EOF on stdin\n",gf_time());
				stdineof = 1;
				if (tooptr == toiptr)
				{
					shutdown(sockfd,SHUT_WR);
				}
			}
			else
			{
				fprintf(stderr,"%s: read %d bytes from stdin\n",gf_time(),n);
				toiptr += n;
				FD_SET(sockfd,&wset);
			}
		}
		
		if (FD_ISSET(sockfd,&rset))
		{
			if ((n = read(sockfd,friptr,&fr[MAXLINE] - friptr)) < 0)
			{
				if (errno != EWOULDBLOCK)
				{
					fprintf(stderr,"%s: read error on socket\n",gf_time());
					exit(-2);
				}
			}
			else if (n == 0)
			{
				fprintf(stderr,"%s: EOF on socket\n",gf_time());
				if (stdineof)
				{
					return;
				}
				else
				{
					fprintf(stderr,"%s: str_cli: server terminated prematurely\n",gf_time());
					return;
				}
			}
			else
			{
				fprintf(stderr,"%s: read %d bytes from socket\n",gf_time(),n);
				friptr += n;
				FD_SET(STDOUT_FILENO,&wset);
			}
		}

		if (FD_ISSET(STDOUT_FILENO,&wset) && ((n = friptr - froptr) > 0))
		{
			if ((nwritten = write(STDOUT_FILENO,froptr,n)) < 0)
			{
				if (errno != EWOULDBLOCK)
				{
					fprintf(stderr,"write error to stdout\n");
					exit(-3);
				}
			}
			else
			{
				fprintf(stderr,"%s: wrote %d bytes to stdout\n",gf_time(),n);
				froptr += nwritten;
				if (froptr == friptr)
				{
					froptr = friptr = fr;
				}
			}
		}

		if (FD_ISSET(sockfd,&wset) && ((n = toiptr - tooptr) > 0))
		{
			if ((nwritten = write(sockfd,tooptr,n)) < 0)
			{
				if (errno != EWOULDBLOCK)
				{
					fprintf(stderr,"write error to socket\n");
					exit(-4);
				}
			}
			else
			{
				fprintf(stderr,"%s: wrote %d bytes to socket\n",gf_time(),nwritten);
				tooptr += nwritten;
				if (tooptr == toiptr)
				{
					toiptr = tooptr = to;
					if (stdineof)
					{
						shutdown(sockfd,SHUT_WR);
					}
				}
			}
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
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
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
	if (connect(sockfd,(struct sockaddr*)(&servaddr),sizeof(servaddr)) < 0)
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

	str_cli(stdin,sockfd);

	/* close sockfd */
	close(sockfd);

	exit(0);
}
