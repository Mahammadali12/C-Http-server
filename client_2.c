#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>


int main()
{
    int client_socket;
    struct sockaddr_in socket_addr;

    memset(&socket_addr,0,sizeof socket_addr);
    socket_addr.sin_port = htons(8080);
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    if( (client_socket = socket(socket_addr.sin_family,SOCK_STREAM,0)) == -1)
    {
        perror("SOCKET()");
    }

    if (connect(client_socket,(struct sockaddr*)&socket_addr,sizeof socket_addr) == -1)
    {
        perror("CONNECT()");
    }

    char* buf = malloc(10);
    printf("%d\n",recv(client_socket,buf,5,0));

    printf("%s\n",buf);
    

}