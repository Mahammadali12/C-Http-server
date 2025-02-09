#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

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

    char* buffer = malloc(512);
    int bytes_received = 0;
    // printf("%d\n",recv(client_socket,buf,5,0));

    while ((bytes_received = recv(client_socket,buffer,512,0)) != 0 )
    {
        printf("\e[1m%d\e[0m bytes were received.\n",bytes_received);

        printf("%s\n",buffer);
        printf("-------\n");
        free(buffer);
        buffer = malloc(512);
    }
    

    // printf("%s\n",buf);
    

}