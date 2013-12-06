#include "unp.h"

#define MAXFILES 20
#define SERV "80"

struct file
{
	char* f_name;
	char* f_host;
	int f_fd;
	int f_flags;
	pthread_t f_tid;
} file[MAXFILES];

#define F_CONNECTING 1
#define F_READING 2
#define F_DONE 4

#define GET_CMD "GET %s/HTTP/1.0\r\n\r\n"

int nconn;
int nfiles;
int nlefttoconn;
int nlefttoread;

void* do_get_read(void*);
void home_page(const char*,const char*);
void write_get_cmd(struct file*);
int min(int,int);

#define F_JOINED 8

int ndone;
pthread_mutex_t ndone_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ndone_cond = PTHREAD_COND_INITIALIZER;

int main(int argc,char* argv[])
{
	int i;
	int maxnconn;

	pthread_t tid;
	struct file* fptr;

	if (argc < 5)
	{
		fprintf(stderr,"usage: %s <#conns> <hostname> <homepage> <file1> ...\n",argv[0]);
		return -1;
	}

	maxnconn = atoi(argv[1]);
	
	nfiles = min(argc - 4,MAXFILES);

	for (i = 0; i < nfiles; i++)
	{
		file[i].f_name = argv[i + 4];
		file[i].f_host = argv[2];
		file[i].f_flags = 0;
	}

	printf("nfiles = %d\n",nfiles);

	home_page(argv[2],argv[3]);
	nlefttoread = nlefttoconn = nfiles;
	nconn = 0;

	while (nlefttoread > 0)
	{
		while (nconn < maxnconn && nlefttoconn > 0)
		{
			for (i = 0; i < nfiles; i++)
			{
				if (file[i].f_flags == 0)
				{
					break;
				}
			}

			if (i == nfiles)
			{
				fprintf(stderr,"nlefttoconn = %d but nothing found\n",nlefttoconn);
				return -1;
			}

			file[i].f_flags = F_CONNECTING;
			pthread_create(&tid,NULL,&do_get_read,&file[i]);
			file[i].f_tid = tid;
			nconn++;
			nlefttoconn--;
		}

		pthread_mutex_lock(&ndone_mutex);
		while (ndone == 0)
		{
			pthread_cond_wait(&ndone_cond,&ndone_mutex);
		}

		for (i = 0; i < nfiles; i++)
		{
			if (file[i].f_flags & F_DONE)
			{
				pthread_join(file[i].f_tid,(void**)&fptr);
				if (&file[i] != fptr)
				{
					fprintf(stderr,"file[i] != fptr\n");
					return -1;
				}
				fptr->f_flags = F_JOINED;
				ndone--;
				nconn--;
				nlefttoread--;
				printf("thread id %d for %s done\n",(int)tid,fptr->f_name);
			}
		}
		pthread_mutex_unlock(&ndone_mutex);
	}

	exit(0);
}

int min(int x,int y)
{
	return x < y ? x : y;
}

void home_page(const char* host,const char* fname)
{
	int fd;
	int n;
	char line[MAXLINE];

	fd = tcp_connect(host,SERV);
	n = snprintf(line,sizeof(line),GET_CMD,fname);
	writen(fd,line,n);

	for (;;)
	{
		if ((n = read(fd,line,MAXLINE)) == 0)
			break;
		printf("read %d bytes of home page\n",n);
		/* do whatever with data */
	}

	printf("end-of-file on home page\n");

	close(fd);
}

void* do_get_read(void* vptr)
{
	int fd;
	int n;
	char line[MAXLINE];
	struct file* fptr;
	
	fptr = (struct file*)vptr;
	
	fd = tcp_connect(fptr->f_host,SERV);
	fptr->f_fd = fd;
	printf("do_get_read for %s,fd %d,thread %d\n",
			fptr->f_name,fd,(int)(fptr->f_tid));
	write_get_cmd(fptr);
	for (;;)
	{
		if ((n = read(fd,line,MAXLINE)) == 0)
		{
			break;
		}
		printf("read %d bytes from %s\n",n,fptr->f_name);
	}
	
	printf("end-of-file on %s\n",fptr->f_name);
	close(fd);

	pthread_mutex_lock(&ndone_mutex);
	fptr->f_flags = F_DONE;
	ndone++;
	pthread_cond_signal(&ndone_cond);
	pthread_mutex_unlock(&ndone_mutex);

	return fptr;
}

void write_get_cmd(struct file* fptr)
{
	int n;
	char line[MAXLINE];

	n = snprintf(line,sizeof(line),GET_CMD,fptr->f_name);
	writen(fptr->f_fd,line,n);
	printf("wrote %d bytes for %s \n",n,fptr->f_name);
	fptr->f_flags = F_READING;
}
