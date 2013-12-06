#include "unp.h"

static int servfd;
static int nsec;
static int maxnprobes;
static int nprobes;
static void sig_urg(int);
static void sig_alrm(int);

void heartbeat_cli(int servfd_arg,int nsec_arg,int maxnprobes_arg)
{
	servfd = servfd_arg;
	if ((nsec = nsec_arg) < 1)
	{
		nsec = 1;
	}

	if ((maxnprobes = maxnprobes_arg) < nsec)
	{
		maxnprobes = nsec;
	}

	nprobes = 0;
	signal(SIGURG,sig_urg);
	fcntl(servfd,F_SETOWN,getpid());
	signal(SIGALRM,sig_alrm);
	
	alarm(nsec);
}

static void sig_urg(int signo)
{
	int n;
	char c;
	
	if ((n = recv(servfd,&c,1,MSG_OOB)) < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			fprintf(stderr,"recv error\n");
			exit(-1);
		}
	}

	nprobes = 0;
	return;
}

static void sig_alrm(int signo)
{
	if (++nprobes > maxnprobes)
	{
		fprintf(stderr,"server is unreachable\n");
		exit(0);
	}

	send(servfd,"1",1,MSG_OOB);
	alarm(nsec);
	return;
}

#define MAXLINE 1024
#define DAYTIME_PORT 5555

int max(int x,int y)
{
	return (x > y) ? x : y;
}

void str_cli(FILE* fp,int sockfd)
{
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	int maxfdl;
	int stdineof;
	fd_set rset;

	stdineof = 0;
	FD_ZERO(&rset);

	heartbeat_cli(servfd,1,5);

	for (;;)
	{
		if (stdineof == 0)
		{
			FD_SET(fileno(fp),&rset);
		}

		FD_SET(sockfd,&rset);
		maxfdl = max(fileno(fp),sockfd) + 1;

		if (select(maxfdl,&rset,NULL,NULL,NULL) < 0)
		{
			if (errno != EINTR)
			{
				fprintf(stderr,"select error: %s\n",strerror(errno));
				return;
			}
			else
			{
				continue;
			}
		}
		
		if (FD_ISSET(sockfd,&rset))
		{
			if (readline(sockfd,recvline,MAXLINE) == 0)
			{
				if (stdineof == 1)
				{
					return;
				}
				else
				{
					fprintf(stderr,"str_cli: server terminated prematurely\n");
					return;
				}
			}
			writen(STDOUT_FILENO,recvline,strlen(recvline));
		}
			
		if (FD_ISSET(fileno(fp),&rset))
		{
			if (fgets(sendline, MAXLINE, fp) == NULL)
			{
				stdineof = 1;
				alarm(0);
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			
			writen(sockfd,sendline,strlen(sendline));
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
	servaddr.sin_port = htons(DAYTIME_PORT);

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

	servfd = sockfd;


	str_cli(stdin,sockfd);

	/* close sockfd */
	close(sockfd);

	exit(0);
}
