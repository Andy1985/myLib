#include "unp.h"

int main(int argc,char* argv[])
{
	int sockfd;
	if (argc != 3)
	{
		fprintf(stderr,"Usage: %s <host> <port#>\n",argv[0]);
		return -1;
	}
	
	sockfd = tcp_connect(argv[1],argv[2]);

	write(sockfd,"123",3);
	printf("wrote 3 bytes of normal data\n");
	//sleep(1);

	send(sockfd,"4",1,MSG_OOB);
	printf("wrote 1 byte of OOB data\n");
	//sleep(1);

	write(sockfd,"56",2);
	printf("wrote 2 bytes of normal data\n");
	//sleep(1);

	send(sockfd,"7",1,MSG_OOB);
	printf("wrote 1 bytes of OOB data\n");
	//sleep(1);

	send(sockfd,"7",1,MSG_OOB);
	printf("wrote 1 bytes of OOB data\n");

	write(sockfd,"89",2);
	printf("wrote 2 bytes of normal data\n");
	//sleep(1);
	
	exit(0);
}
