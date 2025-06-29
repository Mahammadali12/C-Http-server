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
#include <stdlib.h>

#include "list.h"

#define PORT 8080
#define BUFF_SIZE 8192
#define L_BUFF_SIZE 10240

struct http_request_requestLine
{
    char* method;
    char* request_URI;
    char* HTTP_version;
};

struct accept_t
{
    char* mime_type;
    float quality;
};

struct accept_t accepts[10];


int s=0;
struct http_request
{
    char* Host;
    char* Connection;
    char* Accept;  
};

struct http_request_requestLine parseRequestFirstLine(char* request);
void  generateResponseAndSendResponse(struct http_request full_request,struct http_request_requestLine request,char* date,int client_fd);
char** getResources(int* file_count);
void getCurrentTime(char* d);
void parseRequest_TEST(char* request,struct http_request * ht);
void handleClient(int client_fd, int server_fd);
void sendResponse(struct http_request full_request,struct http_request_requestLine request,char* date,int client_fd, int fileFound);
void tokenizeAcceptHeader(char* trimmed_buf, int trimmed_buf_length);


char** files;
int file_count;
char* date;

int main (void)
{
    date = malloc(BUFF_SIZE);
    getCurrentTime(date);

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

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { //* with this function OS does not wait for a few minutes */
        perror("setsockopt(SO_REUSEADDR) failed");                                  // THis function is to fix BIND() problem
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

    files = getResources(&file_count); //!    TODO:   free() array of file names 
    
    printf("\e[33mFiles were counted: %i\e[0m\n",file_count);

    int client_fd;
    while (1)
    {

        struct sockaddr_storage client_address;
        socklen_t addrlen = sizeof(client_address);
        client_fd = accept(socket_fd,(struct sockaddr*)&addr,&addrlen);
        if (client_fd == -1)
        {
            perror("ACCEPT()");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            printf("[child %d] handling client %d\n", getpid(), client_fd);
            close(socket_fd);
            handleClient(client_fd,socket_fd);
            exit(0);
        }else
        {
            printf("[parent %d] spawned child %d for client %d\n", getpid(), pid, client_fd);
            close(client_fd);
        }
        s++;   
    }

    free(date);
    shutdown(socket_fd,SHUT_RDWR);
    close(client_fd);
    close(socket_fd);

    for (size_t i = 0; i < file_count; i++)
    {
        free(*(files+i));
    }
    free(files);
}


void tokenizeAcceptHeader(char* trimmed_buf,int trimmed_buf_length)
{
    char* token;
    char* svptr;
    char* qValue;
    int it = 0;
    int idx = 0;
    float qValueFLoat;
    while (token = strtok_r(trimmed_buf,",",&svptr))
    {
        
        printf("\e[1mTOKEN - %s\e[0m\n",token);
        // printf("\e[1mbuf - %s\e[0m\n",trimmed_buf);
        // printf("\e[1msvp - %s\e[0m\n",svptr);
        accepts[it].mime_type = malloc(200);
        if ((qValue = strstr(token,"q=")) != NULL)
        {
            if (*(qValue+strlen(qValue)-1) == '\r')
            {
                // printf("pox nedi qaqa\n");
                *(qValue+strlen(qValue)-1) = '\0';
                strncpy(accepts[it].mime_type,token,qValue-token);
                
            }else
            {
                strncpy(accepts[it].mime_type,token,qValue-token-1);
            }
            *(accepts[it].mime_type+(qValue-token-1)) = '\0';

            qValue +=2;
            qValueFLoat = strtof(qValue,NULL);


            // printf("%s\n",accepts[it].mime_type);
            
            // printf("qValue-token=%d\n",qValue-token-1);
            
                for (int i = it-idx; i < it; i++)
                {
                    accepts[i].quality = qValueFLoat;
                }
                idx=0;
            
            accepts[it].quality = qValueFLoat;
            
                        
        }else
        {
            // printf("\e[34m %d \e[0m\n",strlen(token));
            strcpy(accepts[it].mime_type,token);
            idx++;
        }
        it++;
        
        trimmed_buf = NULL;
    }

    for (int i = 0; i < it; i++)
    {
        printf("\e[34m Parsed- %s\n\e[0m",accepts[i].mime_type);
        printf("\e[35m Parsed- %f\n\e[0m",accepts[i].quality);
    }
    
    
    printf("%d\n",it);


}

void parseAcceptHeader(char* accept)
{
    int length = 0;
    int i = 0 ;

    if(strncmp(accept,"*/*",3) == 0)
    printf("Accept header is : */*\n");
    else
    {
        printf("Accept header is other type\n");
        // struct accept_t accepts [10];
        int accept_length = strstr(accept,"\n")-accept;
        char* buf = malloc(accept_length);
        strncpy(buf,accept,accept_length);
        int buf_length = strlen(buf);
        char* trimmed_buf = malloc(accept_length);
        int trimmmed_buf_length = 0;
        for (int i = 0; i < buf_length ; i++)
        {
            if (*(buf+i) !=' ')
            {
                *(trimmed_buf+trimmmed_buf_length) = *(buf+i);
                trimmmed_buf_length++;
            }
            
        }

        tokenizeAcceptHeader(trimmed_buf,trimmmed_buf_length);
        // char* token;
        // char* svptr = trimmed_buf;
        // while (token = strtok_r(trimmed_buf,",",&svptr))
        // {   
        //     trimmed_buf = NULL;
        //     printf("\e[1m%s\e[0m\n",token);
        // }
        
        
        printf("\e[31m%s\e[0m\n",buf);
        printf("\e[32m%s\e[0m\n",trimmed_buf);
        while (*(accept+i) != '\n')
        {
            // printf("%d ",*(accept+i));
            i++;
        }
    }
    
    printf("\n");
    printf("\e[1mACCEPT - \e[0m %d\n",i);
}

struct http_request_requestLine parseRequestFirstLine(char* request)
{
    
    struct http_request_requestLine req;
    req.method = malloc(256);
    req.HTTP_version = malloc(256);
    req.request_URI = malloc(256);

    // if(!request || *request == '\0')
    // {
    //     strcpy(req.method, "");
    //     strcpy(req.request_URI, "");
    //     strcpy(req.HTTP_version, "");
    //     return req;
    // }
    
    int flag = 0;
    int finish = 0;
    char* temp_request = request;
    // printf("asdsasa\n");
    while (*(request+2)!='\0')
    {        
        if(*request == '\r' && flag == 2)
        {
            strncpy(req.HTTP_version,temp_request,finish+1); // ? HERE 
            flag++;
            *(req.HTTP_version+finish-1) ='\0'; 
            // temp_request +=1;
            // temp_request = temp_request + finish;
            break;
        }

        if(*request == ' ' && flag == 1)
        {
            strncpy(req.request_URI,temp_request,finish-1);  // ! TODO: why I need -1 in the strncpy() function in order to remove space at the end of string
            *(req.request_URI+finish-1) = '\0';
            flag++;                                             
            // temp_request +=1;
            temp_request = temp_request + finish;
            finish = 0;
        }

        if(*request == ' ' && flag == 0)
        {
            strncpy(req.method,temp_request,finish);
            *(req.method+finish) = '\0'; 
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
    // printf("1--%i\n",strlen(req.method));
    // printf("2--%i\n",strlen(req.request_URI));
    // printf("3--%i\n",strlen(req.HTTP_version));
    return req;
    
}

void sendResponse(struct http_request full_request,struct http_request_requestLine request,char* date,int client_fd, int fileFound)
{

    char* response = malloc(L_BUFF_SIZE);
    char* response_body = malloc(BUFF_SIZE);

    char* file_path = malloc(70);
    int file_dp;

    
    if(fileFound == 1)
    sprintf(file_path,"/home/maqa/C-Http-server/resources/%s",request.request_URI);
    else
    sprintf(file_path,"/home/maqa/C-Http-server/resources/404.html");

    file_dp = open(file_path,O_RDONLY);

    int bytes_read = read(file_dp,response_body,BUFF_SIZE);
    printf("\e[1m%d\e[0m bytes were read to \e[31mresponse body\e[0m\n",bytes_read);

    char* content_type = malloc(50);
    sprintf(content_type,"Content-Type: %s",full_request.Accept);

    if(fileFound == 1)
    sprintf(response,"%s 200 OK\r\n%s\n%s\n\n%s",request.HTTP_version,content_type,date,response_body);
    else
    sprintf(response,"%s 404 NOT FOUND\r\n%s\n%s\n\n%s",request.HTTP_version,content_type,date,response_body);

    int sent_byte = 0;
    if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
    {
        perror("ZERO BYTES WERE SENT");
    }

    printf("\e[1m%d\e[0m bytes were sent to \e[32mresponse\e[0m\n",sent_byte);
    printf("\e[33m RESPONSE \e[0m\n",response);
    printf("--------\n");
    free(response_body);
    free(response);
    response_body = malloc(BUFF_SIZE);
    response = malloc(L_BUFF_SIZE);

    while ( (bytes_read = read(file_dp,response_body,BUFF_SIZE)) != 0)
    {
            printf("\e[1m%d\e[0m bytes were read to \e[31mresponse body\e[0m\n",bytes_read);
            sprintf(response,"%s",response_body);
            if((sent_byte = send(client_fd,response,bytes_read,0)) == -1)
            {
                perror("ZERO BYTES WERE SENT");
                return;
            }
            printf("\e[1m%d\e[0m bytes were sent to \e[32mresponse\e[0m\n",sent_byte);
            printf("\e[33m RESPONSE \e[0m\n",response);
            // printf("\e[33m%s\e[0m\n",response);
            // sleep(1);
            free(response_body);
            free(response);
            response_body = malloc(BUFF_SIZE);
            response = malloc(L_BUFF_SIZE);
    }

    free(response_body);
    free(file_path);
    close(file_dp);
    return;
}

void generateResponseAndSendResponse(struct http_request full_request,struct http_request_requestLine request,char* date,int client_fd)
{     // ! TODO: REMEBER YOU CHANGED RETURN VALUE TO THE VOID    
    for (int i = 0; i < file_count; i++)
    {   
    
        if(strcmp(request.request_URI+1,*(files+i)) == 0) // * +1 for request.request_URI is to remove '/'
        {
            sendResponse(full_request,request,date,client_fd,1);
            printf("\e[32mRequested File was found\e[0m\n");
            return;
        }
    }
    printf("\e[31mRequested File not found\e[0m\n");
    sendResponse(full_request,request,date,client_fd,0);
}

char** getResources(int* file_count)
{


    // int file_count = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir("./resources");   //* Code I ctrl c-v to list files in a directory
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG)
        {
            (*file_count)++;
        }
      }
      closedir(d);
    }

    char** resources;
    resources = malloc(sizeof(char*)*(*file_count));

    int i = 0;
    d = opendir("./resources");   //* Code I ctrl c-v to list files in a directory
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG)
        {
            // printf("%s %i\n", dir->d_name,strlen(dir->d_name));
            *(resources+i) = malloc(strlen(dir->d_name));
            // /   *(resources+i) = dir->d_name;
            strcpy(*(resources+i),dir->d_name);
            // printf("%s %i\n",*(resources+i),strlen(*(resources+i)));
            // printf("%c\n",*(*(resources+i)));
            i++;
        }
      }
      closedir(d);
    }
    return resources;
}

void getCurrentTime(char* d)  // * this function was CTRL C-V from internet do not edit
{
        // Get the current time
    time_t now = time(NULL);
    struct tm *tm = gmtime(&now);

    // Buffer to hold the formatted date
    char date_str[128];
    // Format the time according to RFC 1123
    strftime(date_str, sizeof(date_str), "%a, %d %b %Y %H:%M:%S GMT", tm);

    // Print the formatted date
    // printf("Date: %s\n", date_str);
    strcpy(d,date_str);
}

void parseRequest_TEST(char* received_request,struct http_request * http)
{

    char* request_temp = received_request;
    int i=0;
    int cnt = 0;
    int length = 0;
    while (*request_temp != '\0' )
    {
        if(*(request_temp+1) == '\n' && *(request_temp) == '\r')
        {
            request_temp++;
            request_temp++;
            if(strncmp("Connection",request_temp,10) == 0 )
            {
                for (int i = 0; i < 12; i++)
                request_temp++;

                while( *(request_temp+1) != '\r')
                {
                    request_temp++; 
                    length++;
                }
                http->Connection = malloc(length+2);
                strncpy(http->Connection,request_temp-length,length+1); //* be careful for +1 
                *(http->Connection+length+1) = '\0';
                length = 0;                
                printf("\e[1m Connection was parsed\e[0m\n");
            }
            if (strncmp("Host",request_temp,4) == 0)
            {
                // for (int i = 0; i < 6; i++)
                request_temp+=6;

                while(*(request_temp+1) != '\r')
                {
                    request_temp++;
                    length++;

                }

                http->Host = malloc(length+2);
                strncpy(http->Host,request_temp-length,length+1); //* be careful for +1 
                *(http->Host+length+2) = '\0';
                length = 0;
                printf("\e[1m Host was parsed - %s\e[0m\n", http->Host);
            }
            if(strncmp("Accept:", request_temp, 7) == 0 )
            {
                // for (int i = 0; i < 8; i++)
                request_temp+=8;
                char* accept_body = malloc(BUFF_SIZE);
                parseAcceptHeader(request_temp);
                while (*(request_temp+1) != ',' && *(request_temp+1) != ';' && *(request_temp+1) != ',')
                {


                    request_temp++;
                    length++;
                }

                http->Accept = malloc(length+2);
                strncpy(http->Accept,request_temp-length,length+1);
                *(http->Accept+length+1) = '\0';
                
                length = 0;
                printf("\e[1m Accept was parsed\e[0m\n");
            }
            // else
            // {
                // printf("DALDAN\n");
            // }
            // printf("daldan\n");
            
        }
        request_temp++;
    }
}

void handleClient(int client_fd, int server_fd)
{
    printf("[\e[1;32mCLIENT\e[0m] %d\n[\e[1;32mSERVER\e[0m] %d\n",client_fd,server_fd);
    char* recieved_msg = malloc(BUFF_SIZE);
    int received_byte;

    while(( received_byte = recv(client_fd,recieved_msg,BUFF_SIZE,0)) != -1)   
    {
        if(received_byte == 0)
        return ;
        printf("\e[1m%d\e[0m bytes were received from client: \e[1m%d\e[0m\n",received_byte,client_fd);
        printf("\e[33m%s\e[0m\n",recieved_msg);
        // printf("%d\n",strlen(recieved_msg));
                

        struct http_request_requestLine request_first_line = parseRequestFirstLine(recieved_msg);
        struct http_request request;
        parseRequest_TEST(recieved_msg,&request);
        generateResponseAndSendResponse(request,request_first_line,date,client_fd);

        printf("--------------\n");

        if(!request.Connection)
        printf("Connection: %s %d\n",request.Connection,strlen(request.Connection));
        printf("Host: %s %d\n",request.Host,strlen(request.Host));
        printf("Accept: %s %d\n",request.Accept,strlen(request.Accept));
        printf("%d\n",s);


        if(!request.Connection)
        if(strcmp(request.Connection,"close") == 0)
        break;
        free(recieved_msg);

        close(client_fd);

        // free(response);

        if(!request.Connection)
        free(request.Connection);
        free(request.Host);
        free(request.Accept);

        free(request_first_line.request_URI);
        free(request_first_line.method);
        free(request_first_line.HTTP_version);

    }

}