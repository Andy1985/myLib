#include "unp.h"

static int servfd;
static int nsec;
static int maxnalarms;
static int nprobes;
static void sig_urg(int);
static void sig_alrm(int);

void heartbeat_serv(int servfd_arg,int nsec_arg,int maxnalarms_arg)
{
	servfd = servfd_arg;
	if ((nsec = nsec_arg) < 1)
	{
		nsec = 1;
	}

	if ((maxnalarms = maxnalarms_arg) < nsec)
	{
		maxnalarms = nsec;
	}

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

	send(servfd,&c,1,MSG_OOB);
	nprobes = 0;
	return;
}

static void sig_alrm(int signo)
{
	if (++nprobes > maxnalarms)
	{
		printf("no probes from client\n");
		exit(0);
	}

	alarm(nsec);
	return;
}

#define MAXLINE 1024

void str_echo(int connfd)
{
	ssize_t n;
	char line[MAXLINE];
	
	heartbeat_serv(connfd,1,5);
	for (;;)
	{
		if ((n = readline(connfd,line,MAXLINE)) == 0)
		{
			return;
		}
		writen(connfd,line,n);
	}
}

int main(int argc,char* argv[])
{
	int listenfd;
	int connfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	socklen_t len;
	char buff[MAXLINE];
	
	/* create listenfd */
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	/* init inet addr */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(5555);
	
	/* bind addr & port */
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	/* set listening queue */
	listen(listenfd,LISTENQ);

	for (;;)
	{
		/* blocked & waiting for client to connect */
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&len)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				fprintf(stderr,"accept error\n");
				exit(-1);
			}
		}

		printf("connection from %s,port %d\n",
			inet_ntop(AF_INET,&cliaddr.sin_addr,buff,sizeof(buff)),
			ntohs(cliaddr.sin_port));

		str_echo(connfd);

	}
	
	/* close listenfd */
	close(listenfd);
	exit(0);
}
