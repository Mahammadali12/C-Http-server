#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main ()
{
    struct addrinfo hint;
    struct addrinfo* res;
    char buffer [INET6_ADDRSTRLEN];

    getaddrinfo("www.google.com","80",&hint,&res);

    // struct sockaddr_in* ipv4 = (struct sockaddr_in*) res->ai_addr;    

    // inet_ntop (res->ai_family, &(ipv4->sin_addr), buffer,sizeof(buffer));

    int socket_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

    if(connect(socket_fd,res->ai_addr,res->ai_addrlen) == -1)
    {
        perror("ERROR CONNECTING socket\n");
        return(1);
    }

    
    
}
