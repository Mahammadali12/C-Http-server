#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>


#define PORT 8080
#define BUFF_SIZE 1000

int main (int argc, char** argv)
{
    struct sockaddr_in addr;
    int socket_fd;

    addr.sin_port = htons(PORT);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if ((socket_fd = socket(addr.sin_family,SOCK_STREAM, 0)) == -1)
    {
        perror ("CLIENT SOCKET");
        return(1);
    }
    printf("CLIENT_SOCKET: %d\n",socket_fd);
    
    if (connect(socket_fd,(struct sockaddr*)&addr,sizeof addr) == -1)
    {
        perror ("CONNECT()");
        return(1);
    }

    char* request = malloc(BUFF_SIZE);
    int bytes_sent = 0;
    if(argv[1] == NULL)
    {
        perror("NO URL");
        return(1);
    }

    strcpy(request,argv[1]);

    if((bytes_sent = send(socket_fd,request,strlen(request),0)) == 0)
    {
        perror("MESSAGE WAS NOT SENT");
        return(1);
    }
    printf("message %d bytes were sent\n",bytes_sent);
    printf("MESSAGE:   %s\n",request);

    char* response = malloc(BUFF_SIZE);
    int received_byte = 0;

    if((received_byte = recv(socket_fd,response,BUFF_SIZE,0)) == 0)
    {
        perror("ZERO BYTES WERE RECEIVED AS A RESPONSE");
        return(1);
    }

    printf("%d bytes as a response were received from server\n",received_byte);
    printf("Response: %s\n",response);

    free(request);
    free(response);
}