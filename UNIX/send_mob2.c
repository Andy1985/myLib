#include "unp.h"

int main(int argc,char* argv[])
{
	int sockfd;
	int size;
	char buff[16384];

	if (argc != 3)
	{
		fprintf(stderr,"Usage: %s <host> <port#>\n",argv[0]);
		return -1;
	}
	
	sockfd = tcp_connect(argv[1],argv[2]);
	size = 32768;
	setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&size,sizeof(size));
	
	write(sockfd,buff,16384);
	printf("wrote 16384 bytes of normal data\n");
	sleep(5);
	
	send(sockfd,"a",1,MSG_OOB);
	printf("wrote 1 byte of OOB data\n");

	write(sockfd,buff,1024);
	printf("wrote 1024 bytes of normal data\n");
	

	exit(0);
}
