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


struct http_request_requestLine
{
    char* method;
    char* request_URI;
    char* HTTP_version;
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

    printf("[\e[1;31mSERVER_SOCKET\e[0m]: %d\n",socket_fd);





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

    printf("[\e[1;32mCLIENT\e[0m] %d\n[\e[1;32mSERVER\e[0m] %d\n",client_fd,socket_fd);

    char* recieved_msg = malloc(BUFF_SIZE);
    int received_byte;

    // if((
        received_byte = recv(client_fd,recieved_msg,BUFF_SIZE,0);
        // ) == 0)
    // {       
    //     perror("ZERO BYTES WERE RECEIVED");
    //     return(1);
    // }

    printf("%d bytes were received from client:%d\n",received_byte,client_fd);
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
    printf("Response:\n");
    printf("%s\n",response);
    printf("--------------\n");
    generateResponse(response,recieved_msg);
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
    // int file_dp = open("/home/maqa/C-Http-server/bugg.txt",O_APPEND | O_WRONLY);
    // FILE * fp = fopen("/home/maqa/C-Http-server/bugg.txt","w");
    // if(file_dp == 0)
    // {
    //     printf("ERROR opening file");
    //     return;
    // }
    struct http_request_requestLine daldan;
    daldan.method = malloc(4);
    daldan.HTTP_version = malloc(8);
    daldan.request_URI = malloc(100);
    
    int flag = 0;
    int finish = 0;
    char* temp_request = request;
    while (*(request+2)!='\0')
    {
        
        
        if(*request == '\r' && flag == 2)
        {
            strncpy(daldan.HTTP_version,temp_request,finish);
            flag++;
            temp_request +=1;
            temp_request = temp_request + finish;
            break;
        }

        if(*request == ' ' && flag == 1)
        {
            strncpy(daldan.request_URI,temp_request,finish);
            flag++;
            // temp_request +=1;
            temp_request = temp_request + finish;
            finish = 0;
        }

        if(*request == ' ' && flag == 0)
        {
            strncpy(daldan.method,temp_request,finish);
            flag++;
            temp_request = temp_request + finish;
            temp_request +=1;
            finish = 0;
        }


        // if(*request == ' ' && flag == 2)
        // {
        //     strncpy(daldan.method,temp_request,finish);
        //     flag++;
        //     temp_request = temp_request + finish;
        // }

        finish ++;

        printf("%i ",*request);
        if(*request == '\n')
        printf("%i",'\n');
        request = request+1;
    }
    printf("\n");
    printf("%i\n",finish);
    printf("[\e[36mMETHOD\e[0m] %s\n",daldan.method);
    printf("[\e[31mREQUEST_URI\e[0m] %s\n",daldan.request_URI);
    printf("[\e[32mHTTP_VERSION\e[0m] %s\n",daldan.HTTP_version);

    // do
    // {
    //     printf("%i ",*request);
    //     request = request+1;
        
    // }while (*(request-1) !='\n');
    // printf("\n");
    
    
    // do
    // {
    //     printf("%i ",*request);
    //     request = request+1;
    // }while (*(request-1) !='\n');
    // printf("\n");
    
    // do
    // {
    //     printf("%i ",*request);
    //     request = request+1;   
    // }while (*(request-1) !='\n');
    // printf("\n");
    
    // do
    // {
    //     printf("%i ",*request);
    //     request = request+1;   
    // }while (*(request-1) !='\n');
    // printf("\n");
    

    // do
    // {
    //     printf("%i ",*request);
    //     request = request+1;   
    // }while (*(request-1) !='\n');
    // printf("\n");
    






    // char* buf = malloc(10);
    // int cnt = 0;
    // int init = 0;
    // while (cnt < 4 )
    // {
    //     // fprintf(fp,"\e[31m%c\e[0m",*request);
    //     int read = write(file_dp,request,1);
    //     if( read <= 0)
    //     {
    //         perror ("ERROR WRITING TO THE FILE");
    //     }
    //     printf("%c",*request);
    //     // printf("READ: %d",read);
    //     // printf("%d -- %c\n",*request,*request);
        
    //     // printf("%d",request);
    //     request = request + 1;
    //     if(*request == 10)
    //     {
    //         cnt++;
    //         // printf("%d HERE YOU GO %d\n",init,cnt);
    //         // printf("\e[1m found\e[0m");
    //     }
    //     init++;
    // }
    
}