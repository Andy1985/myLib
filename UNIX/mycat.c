#include "unp.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int my_open(const char* pathname,int mode)
{
	int fd;
	int sockfd[2];
	int status;
	pid_t childpid;
	char c;
	char argsockfd[10];
	char argmode[10];

	socketpair(AF_LOCAL,SOCK_STREAM,0,sockfd);

	if ((childpid = fork()) == 0)
	{
		close(sockfd[0]);
		snprintf(argsockfd,sizeof(argsockfd),"%d",sockfd[1]);
		snprintf(argmode,sizeof(argmode),"%d",mode);
		execl("./openfile","openfile",argsockfd,pathname,argmode,(char*)NULL);

		fprintf(stderr,"execl error\n");
		exit(-1);
	}

	close(sockfd[1]);

	waitpid(childpid,&status,0);
	if (WIFEXITED(status) == 0)
	{
		fprintf(stderr,"child did not terminate\n");
		return -1;
	}

	if ((status = WEXITSTATUS(status)) == 0)
	{
		read_fd(sockfd[0],&c,1,&fd);
	}
	else
	{
		errno = status;
		fd = -1;
	}

	close(sockfd[0]);

	return fd;
}

#define BUFFSIZE 1024

int main(int argc,char* argv[])
{
	int fd;
	int n;
	char buff[BUFFSIZE];

	if (argc != 2)
	{
		fprintf(stderr,"Usage: %s <pathname>\n",argv[0]);
		return -1;
	}

	if ((fd = my_open(argv[1],O_RDONLY)) < 0)
	{
		fprintf(stderr,"connot open %s\n",argv[1]);
		exit(-1);		
	}

	while ((n = read(fd,buff,BUFFSIZE)) > 0)
	{
		write(STDOUT_FILENO,buff,n);
	}

	exit(0);
}
