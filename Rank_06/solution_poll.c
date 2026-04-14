#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h> // atoi, calloc, realloc

#include <stdio.h> // for test

// --- Глобальні змінні для стану сервера ---
struct pollfd	*pfds = NULL;
char			**client_bufs = NULL; // Буфери для повідомлень кожного клієнта
int				sockfd = -1;
int				fd_count = 0;
int				fd_size = 0;

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

void	handle_client_data(int sockfd, int pfd_i, 
	int *fd_count, struct pollfd **pfds) {

	char recv_buf[200000]; // Буфер для одного виклику recv
	//int	sender_fd = pfds[pfd_i].fd;

	size_t len = sizeof(recv_buf);
	// read a message
	// (int sockfd, void *buf, size_t len, int flags);
	int nbytes = recv((*pfds)[pfd_i].fd, recv_buf, len, 0);
	if (nbytes <= 0) {
		if (nbytes == 0)
			printf("server: client %d just left\n", (*pfds)[pfd_i].fd);
		else
			err ("Fatal error");
		char msg_buf[100];
        sprintf(msg_buf, "server: client %d just left\n", sender_fd);
		del_from_pfds(pfds);
		// send message
		// reexamine the slot we just deleted
		(pfd_i)--;
	} else {
		int sender_fd = (*pfds)[pfd_i].fd;

		// 1. Додаємо отримані дані до персонального буфера клієнта
        recv_buf[nbytes] = '\0';
        client_bufs[pfd_i] = str_join(client_bufs[pfd_i], recv_buf);

		// 2. Обробляємо всі повні повідомлення в буфері
        char *msg;
        while (extract_message(&client_bufs[pfd_i], &msg)) {
            // 3. Формуємо фінальне повідомлення з префіксом
            char broadcast_buf[4200]; // Розмір має бути достатнім
            sprintf(broadcast_buf, "client %d: %s", sender_fd, msg);

			for (int i = 0; i < *fd_count; i++) {
				if ((*pfds)[i].fd != sender_fd || (*pfds)[i].fd != sockfd) {
					if (send((*pfds)[i].fd, broadcast_buf, strlen(broadcast_buf), 0) == -1) err ("Fatal error");
				}
			}
			// 5. Звільняємо пам'ять, виділену extract_message
            free(msg);
		}
	}
	
	// if ((*pfds)[*pfd_i].revents & POLLIN) {

	// } else {                /* POLLERR | POLLHUP */
	// 	printf("server: client %d just left\n", (*pfds)[*pfd_i].fd);
	// 	// del_from_pfds(pfds), 
	// }
}

void	del_from_pfds(int pfd_i) {
	(void)pfds;
	close(pfds[pfd_i].fd);
	free(client_bufs[pfd_i]);
    // Зсуваємо останнього клієнта на місце видаленого
    // Це швидше, ніж зсувати весь масив
    if (pfd_i < fd_count - 1) {
        pfds[pfd_i] = pfds[fd_count - 1];
        client_bufs[pfd_i] = client_bufs[fd_count - 1];
    }
    fd_count--;
}

void	add_to_pfds(int connfd, int *fd_count, int *fd_size, struct pollfd **pfds) {
	
	if (*fd_count == *fd_size) {
		*fd_size *= 2; // Double it
		*pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
		// client_bufs
		if (!*pfds) err ("Fatal error");
	}

	int indx = *fd_count;
	(*pfds)[indx].fd = connfd;
	(*pfds)[indx].events = POLLIN;
	// revents ?

	(*fd_count)++; // count connections;
}

void	handle_new_conn(
	int sockfd, int *fd_count, int *fd_size, struct pollfd **pfds) {

	struct sockaddr_in	cli;
	socklen_t			len = sizeof(cli);
	int					connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd == -1) err("Fatal error");
	add_to_pfds(connfd, fd_count, fd_size, pfds);

	char msg_buf[210000];
	sprintf(msg_buf, "server: client %d just arrived\n", connfd);

	for (int i = 0; i < *fd_count; i++) {
		if ((*pfds)[i].fd != sockfd) {
			if (send((*pfds)[i].fd, msg_buf, strlen(msg_buf), 0) == -1) err ("Fatal error");
		}
	}
}

void	process_connections(
	int sockfd, int *fd_count, int *fd_size, struct pollfd **pfds) {
		for (int i = 0; i < *fd_count; i++) {
			if ((*pfds)[i].revents != 0) {
				printf("  fd=%d; events: %s%s%s\n", (*pfds)[i].fd,
                    ((*pfds)[i].revents & POLLIN)  ? "POLLIN "  : "",
                    ((*pfds)[i].revents & POLLHUP) ? "POLLHUP " : "",
                    ((*pfds)[i].revents & POLLERR) ? "POLLERR " : "");
			}
			if ((*pfds)[i].fd == sockfd) {
				handle_new_conn(sockfd, fd_count, fd_size, pfds);
				// to all: "server: client %d just arrived\n"
			} else {
				handle_client_data(sockfd, i, fd_count, pfds);
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
	close(sockfd);
	free (pfds);
	free(client_bufs);
    return 0;
}
