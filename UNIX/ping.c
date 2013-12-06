#include "unp.h"

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define BUFSIZE 1500

char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];

int datalen;
char* host;
int nsent;
pid_t pid;
int sockfd;
int verbose;
void proc_v4(char*,ssize_t,struct timeval*);
void proc_v6(char*,ssize_t,struct timeval*);
void send_v4(void);
void send_v6(void);
void readloop(void);
void sig_alrm(int);
void tv_sub(struct timeval*,struct timeval*);

struct proto
{
	void (*fproc)(char*,ssize_t,struct timeval*);
	void (*fsend)(void);
	struct sockaddr* sasend;
	struct sockaddr* sarecv;
	socklen_t salen;
	int icmpproto;

} *pr;

#ifdef IPV6
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#endif

struct proto proto_v4 = {proc_v4,send_v4,NULL,NULL,0,IPPROTO_ICMP};
#ifdef IPV6
struct proto proto_v6 = {proc_v6,send_v6,NULL,NULL,0,IPPROTO_ICMPV6};
#endif

int datalen = 56;

int main(int argc,char* argv[])
{
	int c;
	struct addrinfo* ai;
	
	opterr = 0;
	while ((c = getopt(argc,argv,"v")) != -1)
	{
		switch(c)
		{
			case 'v': verbose++; break;
			case '?': fprintf(stderr,"unrecognized option: %c\n",c); return -1; break;
		}
	}

	if (optind != argc - 1)
	{
		fprintf(stderr,"Usage: ping [-v] <hostname>\n");
		return -2;
	}

	host = argv[optind];
	pid = getpid();

	signal(SIGALRM,sig_alrm);

	ai = host_serv(host,NULL,0,0);
	printf("PING %s (%s): %d data bytes\n",
		ai->ai_canonname,sock_ntop_host(ai->ai_addr,ai->ai_addrlen),datalen);

	if (ai->ai_family == AF_INET)
	{
		pr = &proto_v4;
	}
#ifdef IPV6
	else if (ai->ai_family == AF_INET6)
	{
		pr = &proto_v6;
		if (IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6*)ai->ai_addr)->sin6_addr)))
		{
			fprintf(stderr,"cannot ping IPv4-mapped IPv6 address\n");
			return -3;
		}
	}
#endif
	else
	{
		fprintf(stderr,"unknown address family %d\n",ai->ai_family);
		return -4;
	}

	pr->sasend = ai->ai_addr;
	pr->sarecv = calloc(1,ai->ai_addrlen);
	pr->salen = ai->ai_addrlen;
	
	readloop();

	exit(0);
}
