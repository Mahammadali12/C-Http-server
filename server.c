#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT "8080"
#define BACKLOG 10

int main(void)
{
    struct addrinfo hints;
    struct addrinfo* res;
    int socket_fd;
    
    
    getaddrinfo(NULL,"80",&hints,&res);

    socket_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

    if(bind(socket_fd,res->ai_addr,res->ai_addrlen) == -1)
    {
        perror("ERROR BINDING THE SOCKET");
    }

    if(listen (socket_fd, 20) == -1)
    {
        perror("ERROR listening");
    }

    

    int client_fd;
    if((client_fd = accept(socket_fd,res->ai_addr,res->ai_addrlen)) == -1)
    {
        perror ("ERROR accepting");
    }

    printf("SUCCESS client: %d  server:%d\n",client_fd,socket_fd);

    
}