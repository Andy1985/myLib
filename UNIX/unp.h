#ifndef UNP_H
#define UNP_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <limits.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

ssize_t readn(int fd,void* vptr,size_t n)
{
	ssize_t nleft;
	ssize_t nread;
	char* ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nread = read(fd,ptr,nleft)) < 0)
		{
			if (errno == EINTR)
			{
				nread = 0;
			}
			else
			{
				return -1;
			}
		}
		else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}

	return (n - nleft);
}

ssize_t writen(int fd,const void* vptr,size_t n)
{
	size_t nleft;
	ssize_t nwriten;
	const char* ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nwriten = write(fd,ptr,nleft)) <= 0)
		{
			if (errno == EINTR)
				nwriten = 0;
			else
				return -1;
		}

		nleft -= nwriten;
		ptr += nwriten;
	}

	return n;
}

ssize_t readline(int fd,void* vptr,size_t maxlen)
{
	ssize_t n;
	ssize_t rc;
	char c;
	
	char* ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		again:
			if ((rc = read(fd,&c,1)) == 1)
			{
				*ptr++ = c;
				if (c == '\n')
					break;
			}
			else if (rc == 0) 
			{
				if (n == 1)
					return 0;
				else
					break;
			}
			else
			{
				if (errno == EINTR)
					goto again;

				return -1;
			}
	}
	
	*ptr = '\0';

	return n;
}

#define MAXLINE 1024
static ssize_t my_read(int fd,char* ptr)
{
	static int read_cnt = 0;
	static char* read_ptr;
	static char read_buf[MAXLINE];
	
	if (read_cnt <= 0)
	{
		again:
			if ((read_cnt = read(fd,read_buf,sizeof(read_buf))) < 0)
			{
				if (errno == EINTR)	
					goto again;
				return -1;
			}
			else if (read_cnt == 0)
			{
				return 0;
			}

			read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;

	return 1;
}

ssize_t my_readline(int fd,void* vptr,size_t maxlen)
{
	int n;
	int rc;
	char c;
	char* ptr;
	
	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		if ((rc = my_read(fd,&c)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0)
		{
			if (n == 1)
				return 0;
			else
				break;
		}
		else
		{
			return -1;
		}
	}

	*ptr = '\0';
	return n;
}

#ifndef S_IFSOCK
#error S_IFSOCK not defined
#endif

int isfdtype(int fd,int fdtype)
{
	struct stat buf;
	if (fstat(fd,&buf) < 0)
		return -1;
	if ((buf.st_mode & S_IFMT) == fdtype)
		return 1;
	else
		return 0;
}

#define MAXSOCKADDR (sizeof(struct sockaddr) * 2)

int sockfd_to_family(int sockfd)
{
	union { struct sockaddr sa; char data[MAXSOCKADDR]; } un;
	socklen_t len;
	len = MAXSOCKADDR;
	if (getsockname(sockfd,(struct sockaddr*)un.data,&len) < 0)
	{
		return -1;
	}

	return (un.sa.sa_family);
}

void get_endian()
{
	union { short s; char c[sizeof(short)]; } un;
	un.s = 0x0102;

	if (sizeof(short) == 2)
	{
		if (un.c[0] == 1 && un.c[1] == 2)
			printf("big-endian\n");
		else if (un.c[0] == 2 && un.c[1] == 1)
			printf("little-endian\n");
		else 
			printf("unknown\n");
	}
	else
	{
		printf("sizeof(short) = %d\n",sizeof(short));
	}
}

void sleep_us(long microseconds)
{
	struct timeval tick;

	tick.tv_sec = microseconds / 1000000;
	tick.tv_usec = microseconds % 1000000;
	
	select(1,NULL,NULL,NULL,&tick);
}

#define OPEN_MAX 1024

#define POLLRDNORM     0x040           /* Normal data may be read.  */
#define POLLRDBAND     0x080           /* Priority data may be read.  */
#define POLLWRNORM     0x100           /* Writing now will not block.  */
#define POLLWRBAND     0x200           /* Priority data may be written.  */

#define INFTIM (-1)

int tcp_connect(const char* hostname,const char* service)
{
	int sockfd;
	int n;
	
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((n = getaddrinfo(hostname,service,&hints,&res)) != 0)
	{
			fprintf(stderr,"tcp_connect error for %s,%s:%s",
			hostname,service,gai_strerror(n));
	}

	ressave = res;
	
	do 
	{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);	
		if (sockfd < 0)
		{
			continue;
		}

		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
		{
			break;
		}

		close(sockfd);
	} while ((res = res->ai_next) != NULL);
	
	if (res == NULL)
	{
		fprintf(stderr,"tcp_connect error for %s,%s\n",hostname,service);
		return -1;
	}

	freeaddrinfo(ressave);
	
	return sockfd;

}

#define LISTENQ 1024

int tcp_listen(const char* host,const char* serv,socklen_t* addrlenp)
{
	int listenfd;
	int n;
	const int on = 1;

	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		fprintf(stderr,"tcp_listen error for %s,%s: %s\n",host,serv,gai_strerror(n));
		return -1;
	}

	ressave = res;

	do
	{
		listenfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (listenfd < 0)
		{
			continue;	
		}

		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
		if (bind(listenfd,res->ai_addr,res->ai_addrlen) == 0)
		{
			break;
		}

		close(listenfd);
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
	{
		fprintf(stderr,"tcp_listen error for %s,%s\n",host,serv);
		return -2;
	}

	listen(listenfd,LISTENQ);
	
	if (addrlenp)
	{
		*addrlenp = res->ai_addrlen;
	}

	freeaddrinfo(ressave);

	return listenfd;
}

int udp_client(const char* host,const char* serv,void** saptr,socklen_t* lenp)
{
	int sockfd;
	int n;
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		fprintf(stderr,"udp_client error for %s,%s: %s",
			host,serv,gai_strerror(n));
		return -1;
	}
	
	ressave = res;

	do
	{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd >= 0)
		{
			break;
		}
	} while ((res = res->ai_next) != NULL);
	
	if (res == NULL)
	{
		fprintf(stderr,"udp_client error for %s,%s\n",host,serv);
		return -2;
	}

	*saptr = malloc(res->ai_addrlen);
	memcpy(*saptr,res->ai_addr,res->ai_addrlen); 
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);
	
	return sockfd;
}

int udp_connect(const char* host,const char* serv)
{
	int sockfd;
	int n;
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		fprintf(stderr,"udp_client error for %s,%s: %s",
			host,serv,gai_strerror(n));
		return -1;
	}
	
	ressave = res;

	do
	{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0)
		{
			continue;
		}
	
		if (connect(sockfd,res->ai_addr,res->ai_addrlen) == 0)
		{
			break;
		}

		close(sockfd);
	} while ((res = res->ai_next) != NULL);
	
	if (res == NULL)
	{
		fprintf(stderr,"udp_client error for %s,%s\n",host,serv);
		return -2;
	}

	freeaddrinfo(ressave);
	return sockfd;
}

int udp_server(const char* host,const char* serv,socklen_t* addrlenp)
{
	int sockfd;
	int n;
	struct addrinfo hints;
	struct addrinfo* res;
	struct addrinfo* ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = getaddrinfo(host,serv,&hints,&res)) != 0)
	{
		fprintf(stderr,"udp_server error for %s,%s: %s\n",
			host,serv,gai_strerror(n));
		return -1;
	}

	ressave = res;

	do
	{
		sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if (sockfd < 0)
		{
			continue;
		}
		
		if (bind(sockfd,res->ai_addr,res->ai_addrlen) == 0)
		{
			break;
		}

		close(sockfd);
	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
	{
		fprintf(stderr,"udp_server error for %s,%s\n",host,serv);
		return -2;
	}

	if (addrlenp)
	{
		*addrlenp = res->ai_addrlen;
	}

	freeaddrinfo(ressave);

	return sockfd;
}

#include <syslog.h>
#include <signal.h>
#define MAXFD 64

void daemon_init(const char* pname,int facility)
{
	int i;
	pid_t pid;
	
	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	setsid();
	signal(SIGHUP,SIG_IGN);
	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	chdir("/");
	umask(0);
	
	for (i = 0; i < MAXFD; i++)
	{
		close(i);
	}

	openlog(pname,LOG_PID,facility);
}

static void connect_alarm(int signo)
{
	return;
}

int connect_timeo(int sockfd,const struct sockaddr* saptr,socklen_t salen,int nsec)
{
	int n;
	
	void ((*sigfunc)(int)) = signal(SIGALRM,connect_alarm);	

	if (alarm(nsec) != 0)
	{
		fprintf(stderr,"connect_timeo: alarm was already set\n");
		return -1;
	}

	if ((n = connect(sockfd,(struct sockaddr*)saptr,salen)) < 0)
	{
		close(sockfd);
		if (errno == EINTR)
		{
			errno = ETIMEDOUT;
		}
	}

	alarm(0);
	signal(SIGALRM,sigfunc);

	return n;
}

int readable_timeo(int fd,int sec)
{
	fd_set rset;
	struct timeval tv;
	
	FD_ZERO(&rset);
	FD_SET(fd,&rset);
	
	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return (select(fd + 1,&rset,NULL,NULL,&tv));
}

#include <sys/un.h>

ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
	struct msghdr	msg;
	struct iovec	iov[1];
	ssize_t			n;
	//int				newfd;

#ifdef	HAVE_MSGHDR_MSG_CONTROL
	union {
	  struct cmsghdr	cm;
	  char				control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr	*cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	msg.msg_accrights = (caddr_t) &newfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ( (n = recvmsg(fd, &msg, 0)) <= 0)
		return(n);

#ifdef	HAVE_MSGHDR_MSG_CONTROL
	if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
	    cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
		if (cmptr->cmsg_level != SOL_SOCKET)
			return -1;
			//err_quit("control level != SOL_SOCKET");
		if (cmptr->cmsg_type != SCM_RIGHTS)
			return -2;
			//err_quit("control type != SCM_RIGHTS");
		*recvfd = *((int *) CMSG_DATA(cmptr));
	} else
		*recvfd = -1;		/* descriptor was not passed */
#else
/* *INDENT-OFF* */
	if (msg.msg_accrightslen == sizeof(int))
		*recvfd = newfd;
	else
		*recvfd = -1;		/* descriptor was not passed */
/* *INDENT-ON* */
#endif

	return(n);
}

ssize_t
write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr	msg;
	struct iovec	iov[1];

#ifdef	HAVE_MSGHDR_MSG_CONTROL
	union {
	  struct cmsghdr	cm;
	  char				control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr	*cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
#else
	msg.msg_accrights = (caddr_t) &sendfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return(sendmsg(fd, &msg, 0));
}

#include <sys/time.h>
#include <time.h>

char* gf_time(void)
{
	struct timeval tv;
	static char str[30];
	char* ptr;

	if (gettimeofday(&tv,NULL) < 0)
	{
		fprintf(stderr,"gettimeofday error\n");
		return NULL;
	}

	ptr = ctime(&tv.tv_sec);
	strcpy(str,&ptr[11]);
	snprintf(str + 8,sizeof(str) - 8,".%06ld",tv.tv_usec);
	
	return str;
}

int connect_nonb(int sockfd,const struct sockaddr* saptr,socklen_t salen,int nsec)
{
	int flags;
	int n;
	int error;
	socklen_t len;
	fd_set rset;
	fd_set wset;
	struct timeval tval;

	flags = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,flags | O_NONBLOCK);
	
	error = 0;
	if ((n = connect(sockfd,saptr,salen)) < 0)
	{
		if (errno != EINPROGRESS)
		{
			return -1;
		}
	}

	if (n == 0)
	{
		goto done;
	}

	FD_ZERO(&rset);
	FD_SET(sockfd,&rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	if ((n = select(sockfd + 1,&rset,&wset,NULL,nsec ? &tval: NULL)) == 0)
	{
		close(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}

	if (FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd,&wset))
	{
		len = sizeof(error);
		if (getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&len) < 0)
		{
			return -1;
		}
	}
	else
	{
		fprintf(stderr,"select error: sockfd not set\n");
		return -1;
	}

done:
	fcntl(sockfd,F_SETFL,flags);
	if (error)
	{
		close(sockfd);
		errno = error;
		return -1;
	}

	return 0;
}

struct addrinfo *
host_serv(const char *host, const char *serv, int family, int socktype)
{
	int				n;
	struct addrinfo	hints, *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
	hints.ai_family = family;		/* AF_UNSPEC, AF_INET, AF_INET6, etc. */
	hints.ai_socktype = socktype;	/* 0, SOCK_STREAM, SOCK_DGRAM, etc. */

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		return(NULL);

	return(res);	/* return pointer to first on linked list */
}

#include <sys/ioctl.h>

#include <sys/param.h>
#ifdef HAVE_SOCKADDR_DL_STRUCT
#include <net/if_dl.h>
#endif

ssize_t
recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
			   struct sockaddr *sa, socklen_t *salenptr, struct in_pktinfo *pktp)
{
	struct msghdr	msg;
	struct iovec	iov[1];
	ssize_t			n;

#ifdef	HAVE_MSGHDR_MSG_CONTROL
	struct cmsghdr	*cmptr;
	union {
	  struct cmsghdr	cm;
	  char				control[CMSG_SPACE(sizeof(struct in_addr)) +
								CMSG_SPACE(sizeof(struct in_pktinfo))];
	} control_un;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	msg.msg_flags = 0;
#else
	bzero(&msg, sizeof(msg));	/* make certain msg_accrightslen = 0 */
#endif

	msg.msg_name = sa;
	msg.msg_namelen = *salenptr;
	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ( (n = recvmsg(fd, &msg, *flagsp)) < 0)
		return(n);

	*salenptr = msg.msg_namelen;	/* pass back results */
	if (pktp)
		bzero(pktp, sizeof(struct in_pktinfo));	/* 0.0.0.0, i/f = 0 */
/* end recvfrom_flags1 */

/* include recvfrom_flags2 */
#ifndef	HAVE_MSGHDR_MSG_CONTROL
	*flagsp = 0;					/* pass back results */
	return(n);
#else

	*flagsp = msg.msg_flags;		/* pass back results */
	if (msg.msg_controllen < sizeof(struct cmsghdr) ||
		(msg.msg_flags & MSG_CTRUNC) || pktp == NULL)
		return(n);

	for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL;
		 cmptr = CMSG_NXTHDR(&msg, cmptr)) {

#ifdef	IP_RECVDSTADDR
		if (cmptr->cmsg_level == IPPROTO_IP &&
			cmptr->cmsg_type == IP_RECVDSTADDR) {

			memcpy(&pktp->ipi_addr, CMSG_DATA(cmptr),
				   sizeof(struct in_addr));
			continue;
		}
#endif

#ifdef	IP_RECVIF
		if (cmptr->cmsg_level == IPPROTO_IP &&
			cmptr->cmsg_type == IP_RECVIF) {
			struct sockaddr_dl	*sdl;

			sdl = (struct sockaddr_dl *) CMSG_DATA(cmptr);
			pktp->ipi_ifindex = sdl->sdl_index;
			continue;
		}
#endif
		//err_quit("unknown ancillary data, len = %d, level = %d, type = %d",
		//		 cmptr->cmsg_len, cmptr->cmsg_level, cmptr->cmsg_type);
		return -1;
	}
	return(n);
#endif	/* HAVE_MSGHDR_MSG_CONTROL */
}

#include <pthread.h>

#include	<sys/resource.h>

#ifndef	HAVE_GETRUSAGE_PROTO
int		getrusage(int, struct rusage *);
#endif

void
pr_cpu_time(void)
{
	double			user, sys;
	struct rusage	myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0)
		//err_sys("getrusage error");
		exit(-1);
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0)
		//err_sys("getrusage error");
		exit(-1);

	user = (double) myusage.ru_utime.tv_sec +
					myusage.ru_utime.tv_usec/1000000.0;
	user += (double) childusage.ru_utime.tv_sec +
					 childusage.ru_utime.tv_usec/1000000.0;
	sys = (double) myusage.ru_stime.tv_sec +
				   myusage.ru_stime.tv_usec/1000000.0;
	sys += (double) childusage.ru_stime.tv_sec +
					childusage.ru_stime.tv_usec/1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
}

#define	MAXN	16384		/* max #bytes that a client can request */

void
web_child(int sockfd)
{
	int			ntowrite;
	ssize_t		nread;
	char		line[MAXLINE], result[MAXN];

	for ( ; ; ) {
		if ( (nread = readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

			/* line from client specifies #bytes to write back */
		ntowrite = atol(line);
		if ((ntowrite <= 0) || (ntowrite > MAXN))
			//err_quit("client request for %d bytes", ntowrite);
			return;

		writen(sockfd, result, ntowrite);
	}
}

#include <sys/mman.h>
static pthread_mutex_t* mptr;

void my_lock_init()
{
	int fd;
	pthread_mutexattr_t mattr;

	fd = open("/dev/zero",O_RDWR,0);
	
	mptr = mmap(0,sizeof(pthread_mutex_t),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

	close(fd);
	
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_setpshared(&mattr,PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mptr,&mattr);
}

void my_lock_wait()
{
	pthread_mutex_lock(mptr);
}

void my_lock_release()
{
	pthread_mutex_unlock(mptr);
}
#endif /* UNP_H */
