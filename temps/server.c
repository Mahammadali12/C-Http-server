#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT "8080"
#define BACKLOG 10

int main(void)
{
    struct addrinfo hints;
    struct addrinfo* res;
    int socket_fd;
    
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    
    if( getaddrinfo(NULL,PORT,&hints,&res) == -1)
    {
        perror("ERROR GETADDRINF()");
    }

    if((socket_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) == -1)
    {
        perror("ERROR CREATIN SOCKET");
    }

    if(bind(socket_fd,res->ai_addr,res->ai_addrlen) == -1)
    {
        perror("ERROR BINDING THE SOCKET");
    }

    if(listen (socket_fd, 20) == -1)
    {
        perror("ERROR listening");
    }

    

    int client_fd;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    if((client_fd = accept(socket_fd,(struct sockaddr*)&client_addr,&addr_size)) == -1)
    {
        perror ("ERROR accepting");
    }

    printf("SUCCESS client: %d  server:%d\n",client_fd,socket_fd);

    freeaddrinfo(res);
}