#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

// --- Глобальні змінні для стану сервера ---
struct pollfd	*pfds = NULL;
char			**client_bufs = NULL; // Буфери для повідомлень кожного клієнта
int				sockfd = -1;
int				fd_count = 0;
int				fd_size = 0;

// --- Допоміжні функції (ваші extract_message та str_join залишаються без змін) ---

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

// --- Основні функції сервера ---

void	broadcast_message(int sender_fd, const char *msg) {
    for (int i = 0; i < fd_count; i++) {
        int dest_fd = pfds[i].fd;
        // Розсилаємо всім, крім слухаючого сокета та самого відправника
        if (dest_fd != sockfd && dest_fd != sender_fd) {
            if (send(dest_fd, msg, strlen(msg), 0) == -1) {
                // У реальному додатку тут варто обробити помилку,
                // наприклад, видалити клієнта, якому не вдалося відправити.
                // Для іспиту можна ігнорувати або викликати err().
            }
        }
    }
}

void	add_client(int connfd) {
    if (fd_count == fd_size) {
        fd_size = (fd_size == 0) ? 5 : fd_size * 2;
        pfds = realloc(pfds, sizeof(*pfds) * fd_size);
        client_bufs = realloc(client_bufs, sizeof(*client_bufs) * fd_size);
        if (!pfds || !client_bufs) err("Fatal error on realloc");
    }
    pfds[fd_count].fd = connfd;
    pfds[fd_count].events = POLLIN;
    client_bufs[fd_count] = NULL; // Ініціалізуємо буфер як порожній
    fd_count++;
}

void	remove_client(int pfd_i) {
    // Закриваємо сокет і звільняємо буфер
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

void	handle_new_conn() {
    struct sockaddr_in	cli;
    socklen_t			len = sizeof(cli);
    int					connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
    if (connfd == -1) return; // Не фатальна помилка, просто ігноруємо

    add_client(connfd);

    char msg_buf[100];
    sprintf(msg_buf, "server: client %d just arrived\n", connfd);
    broadcast_message(connfd, msg_buf); // Розсилаємо всім, крім нового клієнта
}

void	handle_client_data(int pfd_i) {
    char	recv_buf[4096]; // Буфер для одного виклику recv
    int		sender_fd = pfds[pfd_i].fd;

    int nbytes = recv(sender_fd, recv_buf, sizeof(recv_buf), 0);

    if (nbytes <= 0) {
        // Клієнт від'єднався або сталася помилка
        char msg_buf[100];
        sprintf(msg_buf, "server: client %d just left\n", sender_fd);
        remove_client(pfd_i);
        broadcast_message(sender_fd, msg_buf); // Повідомляємо всіх, що клієнт пішов
    } else {
        // 1. Додаємо отримані дані до персонального буфера клієнта
        recv_buf[nbytes] = '\0';
        client_bufs[pfd_i] = str_join(client_bufs[pfd_i], recv_buf);

        // 2. Обробляємо всі повні повідомлення в буфері
        char *msg;
        while (extract_message(&client_bufs[pfd_i], &msg)) {
            // 3. Формуємо фінальне повідомлення з префіксом
            char broadcast_buf[4200]; // Розмір має бути достатнім
            sprintf(broadcast_buf, "client %d: %s", sender_fd, msg);

            // 4. Розсилаємо його всім іншим
            broadcast_message(sender_fd, broadcast_buf);

            // 5. Звільняємо пам'ять, виділену extract_message
            free(msg);
        }
    }
}

int	main(int argc, char **argv) {
    if (argc != 2) err("Wrong number of arguments");

    int port = atoi(argv[1]);
    struct sockaddr_in servaddr;

    // Створення та налаштування слухаючого сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) err("Fatal error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
    servaddr.sin_port = htons(port);

    if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
        err("Fatal error");
    if (listen(sockfd, 10) != 0) err("Fatal error");

    // Додаємо слухаючий сокет до нашого набору
    add_client(sockfd);

    // --- Головний цикл сервера ---
    while(1) {
        if (poll(pfds, fd_count, -1) == -1) err("Fatal error on poll");

        // Перевіряємо події на всіх сокетах
        for (int i = 0; i < fd_count; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd) {
                    // Нове з'єднання
                    handle_new_conn();
                } else {
                    // Дані від існуючого клієнта
                    handle_client_data(i);
                }
            }
        }
    }
    // Код нижче ніколи не виконається, але для повноти...
    close(sockfd);
    free(pfds);
    free(client_bufs);
    return 0;
}