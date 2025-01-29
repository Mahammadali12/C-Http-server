#include <limits.h>
#include <dirent.h>
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
#include <time.h>

#define PORT 8080
#define BUFF_SIZE 1000

struct http_request_requestLine parseRequest(char* request);
void generateResponse(struct http_request_requestLine request);


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
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { /* with this function OS does not for a few minutes */
        perror("setsockopt(SO_REUSEADDR) failed");                                  // THis function is to fic BIND() problem
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

    // char* response = malloc(BUFF_SIZE);
    // int file_dp;
    // file_dp = open("/home/maqa/C-Http-server/resources/index.html", O_RDONLY);
    // printf("%d bytes were read\n",read(file_dp,response,BUFF_SIZE));
    // int sent_byte = 0;
    // if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
    // {
    //     perror("ZERO BYTES WERE SENT");
    //     return(1);
    // }

    // printf("%d bytes were sent as a response to the client(%d)\n",sent_byte,client_fd);
    // printf("Response:\n");
    // printf("%s\n",response);

    printf("--------------\n");
    struct http_request_requestLine request = parseRequest(recieved_msg);
    generateResponse(request);
    printf("--------------\n");

    // close(file_dp);
    // free(response);
    close(client_fd);
    free(recieved_msg);
}

close(socket_fd);
}


struct http_request_requestLine parseRequest(char* request)
{
    
    struct http_request_requestLine req;
    req.method = malloc(sizeof req.method);
    req.HTTP_version = malloc(sizeof req.HTTP_version);
    req.request_URI = malloc(sizeof req.request_URI);
    
    int flag = 0;
    int finish = 0;
    char* temp_request = request;
    while (*(request+2)!='\0')
    {        
        if(*request == '\r' && flag == 2)
        {
            strncpy(req.HTTP_version,temp_request,finish);
            flag++;
            temp_request +=1;
            temp_request = temp_request + finish;
            break;
        }

        if(*request == ' ' && flag == 1)
        {
            strncpy(req.request_URI,temp_request,finish-1);  // ! TODO: why I need -1 in the strncpy() function in order to remove space at the end of string
            flag++;                                             
            // temp_request +=1;
            temp_request = temp_request + finish;
            finish = 0;
        }

        if(*request == ' ' && flag == 0)
        {
            strncpy(req.method,temp_request,finish);
            flag++;
            temp_request = temp_request + finish; 
            temp_request +=1;
            finish = 0;
        }

        finish ++;

        printf("%i ",*request);
        if(*request == '\n')
        printf("%i",'\n');
        request = request+1;
    }
    printf("\n");

    // time_t t;
    // time(&t);   // @bug VERY DANGEROUS!!!! INSPECT BEFORE USING
    // printf("NOW IS %s\n",ctime(&t));

    printf("[\e[36mMETHOD\e[0m] %s\n",req.method);
    printf("[\e[31mREQUEST_URI\e[0m] %s\n",req.request_URI);
    printf("[\e[32mHTTP_VERSION\e[0m] %s\n",req.HTTP_version);
    return req;
    
}


void generateResponse(struct http_request_requestLine request)
{
    if(strcmp(request.request_URI,"/index.html") == 0)
    {
        printf("Here is your %s [\e[1;32mSTATUS CODE\e[0m]: 200 OK\n",request.request_URI+1); // * +1 for request.request_URI is to remove "/"
    }
    else
    {
        printf("you requested %s\n",request.request_URI);
    }

    char** files;
    
    printf("%s\n",files[0]);
    printf("%s\n",files[1]);

    DIR *d;
    struct dirent *dir;
    d = opendir(".");   //* Code I ctrl c-v to list files in a directory
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG)
        {
            printf("%s\n", dir->d_name);        
        }
      }
      closedir(d);
    }

}
