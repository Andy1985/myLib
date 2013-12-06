#include "unp.h"

typedef struct
{
	pthread_t thread_tid;
	long thread_count;
} Thread;

Thread *tptr;

#define MAXNCLI 32
int clifd[MAXNCLI];
int iget;
int iput;

static int nthreads;

pthread_mutex_t clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clifd_cond = PTHREAD_COND_INITIALIZER;

void thread_make(int i);
void* thread_main(void* arg);
void sig_int(int signo);

int main(int argc,char* argv[])
{
	int i;
	int listenfd;
	int connfd;
	socklen_t addrlen;
	socklen_t clilen;
	struct sockaddr* cliaddr;
	
	if (argc == 3)
	{
		listenfd = tcp_listen(NULL,argv[1],&addrlen);
	}
	else if (argc == 4)
	{
		listenfd = tcp_listen(argv[1],argv[2],&addrlen);
	}
	else
	{
		fprintf(stderr,"Usage: %s [<host>] <port#> <#threads>\n",argv[0]);
		return -1;
	}

	cliaddr = (struct sockaddr*)malloc(addrlen);
	nthreads = atoi(argv[argc - 1]);
	tptr = calloc(nthreads,sizeof(Thread));
	iget = iput = 0;
	
	for (i = 0; i < nthreads; i++)
	{
		thread_make(i);
	}

	signal(SIGINT,sig_int);

	for (;;)
	{
		clilen = addrlen;
		connfd = accept(listenfd,cliaddr,&clilen);
		pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;
		if (++iput == MAXNCLI)
		{
			iput = 0;
		}
		if (iput == iget)
		{
			fprintf(stderr,"iput = iget = %d\n",iput);
			return -1;
		}
		pthread_cond_signal(&clifd_cond);
		pthread_mutex_unlock(&clifd_mutex);
	}

	exit(0);
}

void thread_make(int i)
{
	void* thread_main(void*);
	pthread_create(&tptr[i].thread_tid,NULL,&thread_main,(void*)i);
	return;
}

void* thread_main(void* arg)
{
	int connfd;
	printf("thread %d staring\n",(int)arg);

	for (;;)
	{
		pthread_mutex_lock(&clifd_mutex);
		while (iget == iput)
		{
			pthread_cond_wait(&clifd_cond,&clifd_mutex);
		}
		connfd = clifd[iget];
		if (++iget == MAXNCLI)
		{
			iget = 0;
		}
		pthread_mutex_unlock(&clifd_mutex);

		tptr[(int)arg].thread_count++;
		web_child(connfd);
		close(connfd);
	}
}

void sig_int(int signo)
{
	int i;
	void pr_cpu_time(void);
	
	for (i = 0; i < nthreads; i++)
	{
		printf("%d: %ld\n",i,tptr[i].thread_count);
	}

	while (wait(NULL) > 0)
	{
		;
	}

	if (errno != ECHILD)
	{
		fprintf(stderr,"wait error\n");
		exit(-1);
	}

	pr_cpu_time();

	exit(0);
}
