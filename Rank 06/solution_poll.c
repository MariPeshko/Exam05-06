#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

void	err(char *msg) {
	if (msg) write (2, msg, strlen(msg));
	write(2, "\n", 1);
	exit(1);
}

int	main(int argc, char **argv) {
	if (argc < 2) err("Wrong number of arguments");

	int					sockfd;
	struct	sockaddr_in	servaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) err("Fatal error");
	bzero(&servaddr, sizeof(servaddr));
	
}
