#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

int extract_message(char **buf, char **msg) {
	char	*newbuf;
	int		i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i]) {
		if ((*buf)[i] == '\n') {
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0) return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add) {
	char	*newbuf;
	int		len;

	if (buf == 0) len = 0;
	else len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0) return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

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

void	handle_client_data() {
	// resend it to all other clients with 
	// "client %d: " prefixed to every line.
	// OR
	// "server: client %d just left\n".
}

void	del_from_pfds() {}

void	add_to_pfds(int *fd_count, int *fd_size, struct pollfd **pfds) {
	
	*fd_count++;

	int indx = *fd_count - 1;
	struct pollfd	pfds;
	pfds[indx].fd = sockfd;
	pfds[indx].events = POLLIN;
	// if (*fd_count > *fd_size)
	// realloc 
}

void	handle_new_conn(
	int sockfd, int *fd_count, int *fd_size, struct pollfd **pfds) {

	struct sockaddr_in	cli;
	len = sizeof(cli);
	int	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (conndfd == -1) err("Fatal error");
	add_to_pfds(fd_count, fd_size, pollfd);
}

void	process_connections(
	int sockfd, int *fd_count, int *fd_size, struct pollfd **pfds) {
		for (int i = 0; i < *fd_count; i++) {
			if (*pfd[i].revents != 0) {
				printf("  fd=%d; events: %s%s%s\n", pfds[j].fd,
                    (*pfds[j].revents & POLLIN)  ? "POLLIN "  : "",
                    (*pfds[j].revents & POLLHUP) ? "POLLHUP " : "",
                    (*pfds[j].revents & POLLERR) ? "POLLERR " : "");
			}
			if (*pfd[i].fd == sockfd) {
				handle_new_conn(sockfd, fd_count, fd_size, pfds);
				// to all: "server: client %d just arrived\n"
			} else {
				handle_client_data();
			}
		}
}

int	main(int argc, char **argv) {
	if (argc < 2) err("Wrong number of arguments");

	int	port = atoi(argv[1]);
	int	sockfd;

	sockfd = get_listener_socket(port);	
	if (sockfd == -1) err("Fatal error");
	
	struct pollfd	*pfds;
	int				fd_size = 5; // realloc as necessary
	pfds = calloc(fd_size, sizeof(*pfds) * fd_size);
	if (pfds == NULL) err("Fatal error");

	pfds[0].fd = sockfd;
	pfds[0].events = POLLIN;
	int fd_count = 1; // tracks the number of structs in use

	while(1)	{
		int	ready;
		ready = poll(pfds, fd_count, -1);
		if (ready == -1) err("Fatal error");

		process_connections(sockfd, &fd_count, &fd_size, &pfds);
	}

	free (pfds);
}
