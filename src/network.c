#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int server_listen(int port, int backlog) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return -1; }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_LOOPBACK), .sin_port = htons(port) };
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(fd); return -1; }

    if (listen(fd, backlog) < 0) { perror("listen"); close(fd); return -1; }
    return fd;
}

int accept_connection(int server_fd, struct sockaddr_in *client_addr) {
    socklen_t len = sizeof(*client_addr);
    return accept(server_fd, (struct sockaddr*)client_addr, &len);
}

void server_shutdown(int server_fd) {
    shutdown(server_fd, SHUT_RDWR);
    close(server_fd);
}