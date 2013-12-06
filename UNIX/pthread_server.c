#include "unp.h"

typedef struct
{
	pthread_t thread_tid;
	long thread_count;
} Thread;

Thread *tptr;

int listenfd;
int nthreads;
socklen_t addrlen;
pthread_mutex_t mlock_ = PTHREAD_MUTEX_INITIALIZER;

void thread_make(int i);
void* thread_main(void* arg);
void sig_int(int signo);

int main(int argc,char* argv[])
{
	int i;
	
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

	nthreads = atoi(argv[argc - 1]);
	tptr = calloc(nthreads,sizeof(Thread));
	
	for (i = 0; i < nthreads; i++)
	{
		thread_make(i);
	}

	signal(SIGINT,sig_int);

	for (;;)
	{
		pause();
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
	socklen_t clilen;
	struct sockaddr* cliaddr;
	
	cliaddr = (struct sockaddr*)malloc(addrlen);
	printf("thread %d staring\n",(int)arg);
	for (;;)
	{
		clilen = addrlen;
		pthread_mutex_lock(&mlock_);
		connfd = accept(listenfd,cliaddr,&clilen);
		pthread_mutex_unlock(&mlock_);
		tptr[(int)arg].thread_count++;
		web_child(connfd);
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

	pr_cpu_time();

	exit(0);
}
