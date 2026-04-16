#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h> // atoi, calloc, realloc
#include <stdio.h> // for test

int client_ids[1024];
int next_id = 0;

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

void	broadcast_msg(int sender_fd, const char *msg) {
    for (int i = 0; i < fd_count; i++) {
        int dest_fd = pfds[i].fd;
        if (dest_fd != sockfd && dest_fd != sender_fd) {
            if (send(dest_fd, msg, strlen(msg), 0) == -1) {
                printf("broadcast_msg: send err\n");
            }
        }
    }
}

int	get_listener_socket(int port)
{
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
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		err("Fatal error");
	else
		printf("Socket successfully bound.\n");
	if (listen(sockfd, 128) != 0) err("Fatal error");
	return sockfd;
}

// pfd_i - index of teh client in pfds array
void	remove_client(int pfd_i) {
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

// Connfd - socket. fd_count - total number of sockets
void	add_client(int connfd) {
	if (fd_count == fd_size) {
		if (fd_size == 0) {
			fd_size = 5;
		} else {
			fd_size = fd_size * 2;
		}
		pfds = realloc(pfds, sizeof(*pfds) * (fd_size));
		client_bufs = realloc(client_bufs, sizeof(*client_bufs) * (fd_size));
		if (!pfds || !client_bufs) err ("Fatal error");

	}
	pfds[fd_count].fd = connfd;
	pfds[fd_count].events = POLLIN;
	// TO DO
	pfds[fd_count].revents = 0;
	client_bufs[fd_count] = NULL; // Ініціалізуємо буфер як порожній
	fd_count++;
}

// повертає 1, якщо видалив
int	handle_client_data(int pfd_i) {
	char recv_buf[1024]; // Буфер для одного виклику recv
	int sender_fd = pfds[pfd_i].fd;

	int nbytes = recv(sender_fd, recv_buf, sizeof(recv_buf) - 1, 0);
	if (nbytes <= 0) {
		char msg_buf[100];
        sprintf(msg_buf, "server: client %d just left\n", client_ids[sender_fd]);
		remove_client(pfd_i);
		broadcast_msg(sender_fd, msg_buf); // Повідомляємо всіх, що клієнт пішов
		return 1;
	} else {
        recv_buf[nbytes] = '\0';
        // 1. Додаємо отримані дані до персонального буфера клієнта
        client_bufs[pfd_i] = str_join(client_bufs[pfd_i], recv_buf);

		// 2. Обробляємо всі повні повідомлення в буфері
        char *msg = NULL;
		while (extract_message(&client_bufs[pfd_i], &msg)) {
            char broadcast_buf[4200];
            sprintf(broadcast_buf, "client %d: ", client_ids[sender_fd]);
			// 3. Розсилаємо префікс
        	broadcast_msg(sender_fd, broadcast_buf);
			// 4. Розсилаємо повідомлення
        	broadcast_msg(sender_fd, msg);
			// 5. Звільняємо пам'ять, виділену extract_message
            free(msg);
		}
		return 0;
	}
	return 0;
}

void	handle_new_conn() {
	struct sockaddr_in	cli;
	socklen_t			len = sizeof(cli);
	int					connfd;
	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd < 0) return; // Не фатальна помилка, просто ігноруємо
	
	add_client(connfd);
	client_ids[connfd] = next_id;
	next_id++;

	char	msg_buf[100];
	sprintf(msg_buf, "server: client %d just arrived\n", client_ids[connfd]);
	broadcast_msg(connfd, msg_buf);
}

int	main(int argc, char **argv) {
	if (argc < 2) err("Wrong number of arguments");

	int	port = atoi(argv[1]);

	sockfd = get_listener_socket(port);	
	if (sockfd == -1) err("Fatal error");
	
	add_client(sockfd);

	while(1) {
		int	ready = poll(pfds, fd_count, -1);
		if (ready == -1) err("Fatal error");
		for (int i = 0; i < fd_count; i++) {
			if (pfds[i].revents & POLLIN) {
				if (pfds[i].fd == sockfd) 
					handle_new_conn();
				else {
					if (handle_client_data(i) == 1)
						i--;
				}
			}
		}
	}
	close(sockfd);
	free(pfds);
	free(client_bufs);
    return 0;
}
