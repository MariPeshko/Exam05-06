#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

typedef struct s_client {
	int		id;
	char	msg[210000];
} t_client;

t_client	clients[2048];
/* fd_set is a special data structure used by the select() system call. Think of 
it as a collection or a set of file descriptors (like your sockets). You use 
it to tell select() which file descriptors you want to monitor for activity 
(e.g., "is there data to be read from any of these clients?"). */
fd_set		write_set, read_set, current;
int			max_fd = 0, gid = 0;
char		send_buf[210000], recv_buf[210000];

void	err(char *msg) {
	if (msg) write(2, msg, strlen(msg));
	else write(2, "Fatal error", 11);
	write(2, "\n", 1);
	exit(1);
}

void send_all(int except) {
	for (int fd = 0; fd <= max_fd; fd++) {
		if (FD_ISSET(fd, &write_set) && fd != except)
		send(fd, send_buf, strlen(send_buf), 0);
	}
}

int main(int ac, char **av) {
	if (ac != 2) err("Wrong number of arguments");
	
	struct	sockaddr_in addr;
	int		sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) err(NULL);

	max_fd = sockfd;
	FD_ZERO(&current);
	FD_SET(sockfd, &current);
	
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	addr.sin_port = htons(atoi(av[1]));
	
	if (bind(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) err(NULL);
	if (listen(sockfd, 128) < 0) err(NULL);
	
	while (1) {
		read_set = write_set = current;
		if (select(max_fd + 1, &read_set, &write_set, 0, 0) < 0) continue;
		
		for (int fd = 0; fd <= max_fd; fd++) {
			if (FD_ISSET(fd, &read_set)) {
				// CASE 1: NEW CLIENT ARRIVING
				if (fd == sockfd) {
					int connfd = accept(sockfd, NULL, NULL);
					if (connfd < 0) continue;
					if (connfd > max_fd) max_fd = connfd;
					clients[connfd].id = gid++;
					bzero(clients[connfd].msg, sizeof(clients[connfd].msg));
					FD_SET(connfd, &current);
					sprintf(send_buf, "server: client %d just arrived\n", clients[connfd].id);
					send_all(connfd);
					// NO BREAK
				}
				// CASE 2: EXISTING CLIENT SENDING DATA OR LEAVING
				else {
					int ret = recv(fd, recv_buf, sizeof(recv_buf) - 1, 0);
					if (ret <= 0) {
						sprintf(send_buf, "server: client %d just left\n", clients[fd].id);
						send_all(fd);
						FD_CLR(fd, &current);
						close(fd);
						bzero(clients[fd].msg, sizeof(clients[fd].msg));
						// NO BREAK
					}
					else {
						for (int i = 0, j = strlen(clients[fd].msg); i < ret; i++, j++) {
							clients[fd].msg[j] = recv_buf[i];
							clients[fd].msg[j + 1] = '\0'; // ALWAYS NULL TERMINATE
							if (clients[fd].msg[j] == '\n') {
								clients[fd].msg[j] = '\0';
								sprintf(send_buf, "client %d: %s\n", clients[fd].id, clients[fd].msg);
								send_all(fd);
								bzero(clients[fd].msg, sizeof(clients[fd].msg));
								j = -1; // RESET BUFFER POINTER
							}
						}
						// NO BREAK
					}
				}
			}
		}
	}
}
