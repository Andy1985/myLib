#include "unp.h"

int main(int argc,char* argv[])
{
	int fd;
	ssize_t n;
	
	if (argc != 4)
	{
		fprintf(stderr,"Usage: %s <sockfd#> <filename> <mode>\n",argv[1]);
		exit(-1);
	}

	if ((fd = open(argv[2],atoi(argv[3]))) < 0)
	{
		exit(errno > 0 ? errno: 255);
	}

	if ((n = write_fd(atoi(argv[1]),"",1,fd)) < 0)
	{
		exit(errno > 0 ? errno : 255);
	}

	exit(0);
}
