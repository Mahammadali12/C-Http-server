#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <malloc.h>


#define PORT 8080
#define BUFF_SIZE 1000
int main (void)
{
    int socket_fd;
    struct sockaddr_in addr;

    memset(&addr,0,sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(PORT);

    // struct addrinfo* res;
    // struct addrinfo hints = {0};

    // hints.ai_family = AF_INET;

    // if (getaddrinfo(NULL,"8080",&hints,&res) == -1)
    // {
    //     perror("GETADDRINFO()");
    // }

    
    if((socket_fd = socket(addr.sin_family,SOCK_STREAM,0)) == -1)
    {
        perror ("SOCKET()");
        return(1);
    }

    printf("SERVER_SOCKET: %d\n",socket_fd);

    if( bind(socket_fd,(struct sockaddr *)&addr,sizeof addr) == -1)
    {
        perror("BIND()");
        return(1);
    }

    if( listen(socket_fd,20) == -1)
    {
        perror("LISTEN()");
        return(1);
    }

    int client_fd;
    struct sockaddr_storage dd;
    socklen_t addrlen;
    if ((client_fd = accept(socket_fd,(struct sockaddr*)&addr,&addrlen)) == -1)
    {
        perror("ACCEPT()");
        return(1);
    }

    printf("CLIENT %d SERVER %d\n",client_fd,socket_fd);

    char* recieved_msg = malloc(BUFF_SIZE);
    int received_byte;
    if((received_byte = recv(client_fd,recieved_msg,BUFF_SIZE,0)) == 0)
    {       
        perror("ZERO BYTES WERE RECEIVED");
        return(1);
    }
    printf("%d bytes were received from client: %d\n",received_byte,client_fd);
    printf("MESSAGE: %s\n",recieved_msg);

    char* response = malloc(BUFF_SIZE);
    response = "XUY";
    int sent_byte = 0;
    if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
    {
        perror("ZERO BYTES WERE SENT");
        return(1);
    }

    printf("%d bytes were sent as a response to the client(%d)\n",sent_byte,client_fd);
    printf("Response: %s\n",response);


}