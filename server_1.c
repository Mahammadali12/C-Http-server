#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT "8080"
int main (void)
{
    int socket_fd;
    struct sockaddr_in addr;

    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    if( socket_fd = socket(addr.sin_family,&addr.sin_addr,sizeof addr.sin_addr) == -1)
    {
        perror ("BIND()");
    }
}