#include "unp.h"

static int sockfd;

#define QSIZE 8
#define MAXDG 4096

typedef struct
{
	void *dg_data;
	size_t dg_len;
	struct sockaddr* dg_sa;
	socklen_t dg_salen;
} DG;

static DG dg[QSIZE];

static long cntread[QSIZE + 1];

static int iget;
static int iput;
static int nqueue;
static socklen_t clilen;

static void sig_io(int);
static void sig_hup(int);

void dg_echo(int sockfd_arg,struct sockaddr* pcliaddr,socklen_t clilen_arg)
{
	int i;
	const int on = 1;
	sigset_t zeromask;
	sigset_t newmask;
	sigset_t oldmask;

	sockfd = sockfd_arg;
	clilen = clilen_arg;

	for (i = 0; i < QSIZE; i++)
	{
		dg[i].dg_data = malloc(MAXDG);
		dg[i].dg_sa = malloc(clilen);
		dg[i].dg_salen = clilen;
	}

	iget = iput = nqueue = 0;
	
	signal(SIGHUP,sig_hup);
	signal(SIGIO,sig_io);
	fcntl(sockfd,F_SETOWN,getpid());
	ioctl(sockfd,FIOASYNC,&on);
	ioctl(sockfd,FIONBIO,&on);
	sigemptyset(&zeromask);
	sigemptyset(&oldmask);
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGIO);
	
	sigprocmask(SIG_BLOCK,&newmask,&oldmask);
	
	for (;;)
	{
		while (nqueue == 0)
		{
			sigsuspend(&zeromask);
		}

		sigprocmask(SIG_SETMASK,&oldmask,NULL);
		sendto(sockfd,dg[iget].dg_data,dg[iget].dg_len,0,dg[iget].dg_sa,dg[iget].dg_salen);
		if (++iget >= QSIZE)
		{
			iget = 0;
		}

		sigprocmask(SIG_BLOCK,&newmask,&oldmask);
		nqueue--;
	}
}

static void sig_io(int signo)
{
	ssize_t len;
	int nread;
	DG* ptr;

	for (nread = 0;;)
	{
		if (nqueue >= QSIZE)
		{
			fprintf(stderr,"receive overflow\n");
			return;
		}

		ptr = &dg[iput];
		ptr->dg_salen = clilen;
		len = recvfrom(sockfd,ptr->dg_data,MAXDG,0,ptr->dg_sa,&ptr->dg_salen);
		if (len < 0)
		{
			if (errno == EWOULDBLOCK)
			{
				break;
			}
			else
			{
				fprintf(stderr,"recvfrom error\n");
				exit(-1);
			}
		}
		ptr->dg_len = len;
		
		nread++;
		nqueue++;
		
		if (++iput >= QSIZE)
		{
			iput = 0;
		}
	}

	cntread[nread]++;
}

static void sig_hup(int signo)
{
	int i;
	for (i = 0; i <= QSIZE; i++)
	{
		printf("cntread[%d] = %ld\n",i,cntread[i]);
	}
}

int main(int argc,char* argv[])
{
	int sockfd_main;

	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;

	/* create sockfd */
	sockfd_main = socket(AF_INET,SOCK_DGRAM,0);

	/* init inet addr */
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);
	
	/* bind addr & port */
	bind(sockfd_main,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	/* proceed client data*/
	dg_echo(sockfd_main,(struct sockaddr*)&cliaddr,sizeof(cliaddr));

	/* close sockfd */
	close(sockfd_main);
	exit(0);
}
