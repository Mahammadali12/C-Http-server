#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <malloc.h>
#include <fcntl.h>

#define PORT 8080
#define BUFF_SIZE 1000

void generateResponse(char* response,char* request);


struct http_response
{
    char* status_line;
    char* general_header;
    char* response_heade;
    char* entity_header;
    char* entity_body;
};




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





    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        return(1);
    }


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
while (1)
{
    /* code */


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
    // if((
        received_byte = recv(client_fd,recieved_msg,BUFF_SIZE,0);
        // ) == 0)
    // {       
    //     perror("ZERO BYTES WERE RECEIVED");
    //     return(1);
    // }
    printf("%d bytes were received from client: %d\n",received_byte,client_fd);
    printf("MESSAGE:\n");
    printf("%s\n",recieved_msg);

    char* response = malloc(BUFF_SIZE);
    int file_dp;
    file_dp = open("/home/maqa/C-Http-server/index.html", O_RDONLY);
    printf("%d bytes were read\n",read(file_dp,response,BUFF_SIZE));
    int sent_byte = 0;
    if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
    {
        perror("ZERO BYTES WERE SENT");
        return(1);
    }

    printf("%d bytes were sent as a response to the client(%d)\n",sent_byte,client_fd);
    printf("Response:\n",response);
    printf("%s\n",response);
    printf("--------------\n");
    generateResponse(response,recieved_msg);
    printf("\n");
    printf("--------------\n");

    close(file_dp);
    free(response);
    close(client_fd);
    free(recieved_msg);
}

close(socket_fd);
}


void generateResponse(char* response,char* request)
{
    int file_dp = open("/home/maqa/C-Http-server/bugg.txt",O_APPEND | O_WRONLY);
    // FILE * fp = fopen("/home/maqa/C-Http-server/bugg.txt","w");
    if(file_dp == 0)
    {
        printf("yarrrraaa");
        return;
    }
    // char* buf = malloc(10);
    int cnt = 0;
    int init = 0;
    while (cnt < 4)
    {
        // fprintf(fp,"\e[31m%c\e[0m",*request);
        int read = write(file_dp,request,1);
        if( read <= 0)
        {
            perror ("ERROR WRITING TO THE FILE");
        }
        // printf("READ: %d",read);
        // printf("%d -- %c\n",*request,*request);
        
        // printf("%d",request);
        request = request + 1;
        if(*request == 10)
        {
            cnt++;
            printf("\n");
            printf("%d HERE YOU GO %d\n",init,cnt);
        }
        init++;
    }
    
}