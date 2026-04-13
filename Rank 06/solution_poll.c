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

int	get_listener_socket(int port)
{
	int					sockfd;
	struct	sockaddr_in	servaddr;

	bzero(&servaddr, sizeof(servaddr));
	// assign IP, PORT 
    servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(port); 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) err("Fatal error");
	else
		printf("Socket successfully created..\n");
	
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		err("Fatal error");
	else
		printf("Socket successfully bound.\n");
	
	if (listen(sockfd, 10) != 0) err("Fatal error");

	return sockfd;
}

int	main(int argc, char **argv)
{
	if (argc < 2) err("Wrong number of arguments");

	int	port = atoi(argv[1]);
	int	sockfd;

	sockfd = get_listener_socket(port);	
	if (sockfd == -1) err("Fatal error");
	
	struct pollfd	*pfds;
	int				fd_size = 5; // realloc as necessary
	pfds = calloc(sizeof(pfds), sizeof(struct pollfd) * fd_size);

	// Main loop
	// {
	// 	process_connections(sockfd, ..., &fd_size, &pfds);
	// }
}
