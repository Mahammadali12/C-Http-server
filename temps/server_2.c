#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>


int main()
{

    int server_socket;
    struct sockaddr_in socket_addr;

    memset(&socket_addr,0,sizeof socket_addr);
    socket_addr.sin_port = htons(8080);
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    if( (server_socket = socket(socket_addr.sin_family,SOCK_STREAM,0)) == -1)
    {
        perror("SOCKET()");
    }



    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //* with this function OS does not wait for a few minutes */
        perror("setsockopt(SO_REUSEADDR) failed");                                  // THis function is to fix BIND() problem
        return(1);
    }


    printf("SERVER: %d\n",server_socket);

    if( bind(server_socket,(struct sockaddr*)&socket_addr,sizeof socket_addr) == -1 )
    {
        perror("BIND()");
    }

    if (listen(server_socket,20) == -1)
    {
        perror("LISTEN()");
    }

    int client_socket;    
    struct sockaddr_storage client_addr;
    socklen_t client_socklen_t;
    if((client_socket = accept(server_socket,(struct sockaddr*)&client_addr,&client_socklen_t)) == -1)
    {
        perror("ACCEPT()");
    }

    printf("CLIENT: %d\n",client_socket);




    int file_dp = open("/home/maqa/C-Http-server/resources/test.txt", O_RDONLY);
    int bytes_read = 0;
    char* buffer = malloc(512);


    while ((bytes_read = read(file_dp,buffer,512)) != 0)
    {
        
        printf("\e[1m%d\e[0m bytes were read\n",bytes_read);

        // printf("%s\n",buffer);
        printf("\e[1m%d\e[0m bytes were sent\n",send(client_socket,buffer,strlen(buffer),0));

        printf("--------\n");

        free(buffer);

        buffer = malloc(512);

        sleep(3);
    }
    
    free(buffer);




}