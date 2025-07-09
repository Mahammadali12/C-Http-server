#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h>

// Start listening on `port`, return listening socket fd or -1 on error.
int  server_listen(int port, int backlog);

// Accept a client connection (blocking) and return client fd.
int  accept_connection(int server_fd, struct sockaddr_in *client_addr);

// Clean up sockets
void server_shutdown(int server_fd);

#endif // NETWORK_H