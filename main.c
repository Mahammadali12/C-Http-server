#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "network.h"
#include "request.h"
#include "response.h"
#include "utils.h"

#define PORT     8080
#define BACKLOG  20
#define RECV_BUF 8192

static int server_fd;

void handle_sigint(int signo) {
    (void)signo;
    // fprintf(stderr, "\nShutting down server...\n");
    LOG("INFO", "SIGINT received, shutting down");
    server_shutdown(server_fd);
    exit(0);
}

int main(void) {
    signal(SIGINT, handle_sigint);
    LOG("INFO", "Initializing server...");

    server_fd = server_listen(PORT, BACKLOG);
    if (server_fd < 0) {
        // fprintf(stderr, "Failed to start server on port %d\n", PORT);
        LOG("ERROR", "Failed to start server on port %d", PORT);
        return EXIT_FAILURE;
    }
    // printf("Server listening on port %d\n", PORT);
    LOG("INFO", "Server listening on port %d", PORT);
    for (;;) {
        struct sockaddr_in client_addr;
        int client_fd = accept_connection(server_fd, &client_addr);
        if (client_fd < 0) {
            // perror("accept_connection");
            LOG("ERROR", "accept_connection failed: %s", strerror(errno));
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        LOG("DEBUG", "Accepted %s:%d (fd=%d)",
            ip, ntohs(client_addr.sin_port), client_fd);

        pid_t pid = fork();
        if (pid < 0) {
            // perror("fork");
            LOG("ERROR", "fork() failed: %s", strerror(errno));
            close(client_fd);
            continue;
        }
        if (pid == 0) {
            close(server_fd);
            LOG("DEBUG", "Child %d handling client %d", getpid(), client_fd);

            char buf[RECV_BUF];
            ssize_t bytes = recv(client_fd, buf, sizeof(buf)-1, 0);
            if (bytes > 0) {
                LOG("DEBUG", "Read %zd bytes from client %d", bytes, client_fd);


                buf[bytes] = '\0';
                http_request_t req;
                if (parse_http_request(buf, &req) == 0) {
                    LOG("INFO", "Parsed request: %s %s %s",req.method, req.uri, req.version);
                    send_response(client_fd, &req);
                    LOG("INFO", "Response sent to client %d", client_fd);
                } else {
                    LOG("ERROR", "Failed to parse request from client %d", client_fd);
                    send_error(client_fd, 400);
                }
            }else {
                LOG("DEBUG", "recv() returned %zd for client %d", bytes, client_fd);
            }
            
            close(client_fd);
            LOG("DEBUG", "Child %d exiting", getpid());
            exit(EXIT_SUCCESS);
        } else {
            //Parent
            LOG("DEBUG", "Parent %d closed fd %d and continues", getpid(), client_fd);
            close(client_fd);
        }
    }

    server_shutdown(server_fd);
    return EXIT_SUCCESS;
}
