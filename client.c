#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main (void)
{
    struct sockaddr_in addr;
    int socket_fd;

    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if ((socket_fd = socket(addr.sin_family,SOCK_STREAM, 0)) == -1)
    {
        perror ("CLIENT SOCKET");
    }
    printf("SOCKET %d\n",socket_fd);
    
    if (connect(socket_fd,(struct sockaddr*)&addr,sizeof addr) == -1)
    {
        perror ("CONNECT()");
    }

    
    

}