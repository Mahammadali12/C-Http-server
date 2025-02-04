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
char* generateResponse(struct http_request_requestLine request,char** files,int file_count,char* date);
char** getResources(int* file_count);
void getCurrentTime(char* d);
struct http_request parseRequest_TEST(char* request);

struct http_request_requestLine
{
    char* method;
    char* request_URI;
    char* HTTP_version;
};


struct http_request
{
    char* Host;
    char* Connection;
};



int main (void)
{
    char* date = malloc(128);
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

    printf("[\e[1;31mSERVER_SOCKET\e[0m]: %d\n",socket_fd);


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

    int file_count = 0;
    char** files = getResources(&file_count); //!    TODO:   free() array of file names 
    printf("\e[33mFiles were counted: %i\e[0m\n",file_count);

    // while (1)
    // {

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
        //     ) == 0)
        // {       
        //     perror("ZERO BYTES WERE RECEIVED");
        //     return(1);
        // }

        printf("%d bytes were received from client:%d\n",received_byte,client_fd);
        printf("MESSAGE:\n");
        printf("%s\n",recieved_msg);

        char* response;
        // int file_dp;
        // file_dp = open("/home/maqa/C-Http-server/resources/index.html", O_RDONLY);
        // printf("%d bytes were read\n",read(file_dp,response,BUFF_SIZE));

        struct http_request_requestLine request = parseRequest(recieved_msg);

        response = generateResponse(request,files,file_count,date);
        int sent_byte = 0;
        if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
        {
            perror("ZERO BYTES WERE SENT");
            return(1);
        }

        printf("%d bytes were sent as a response to the client(%d)\n",sent_byte,client_fd);
        printf("Response:\n");
        printf("%s",response);

        printf("--------------\n");

        printf("--------------\n");
        struct http_request rq = parseRequest_TEST(recieved_msg);
        printf("Connection: %s\n",rq.Connection);
        printf("Host: %s\n",rq.Host);
        // if(strcmp(rq.Connection,"close") == 0)
        // break;
        // close(file_dp);
        // free(response);
        close(client_fd);

        free(recieved_msg);

        free(response);

        free(rq.Connection);
        free(rq.Host);

        free(request.request_URI);
        free(request.method);
        free(request.HTTP_version);
        // free(request.HTTP_version);
    // }


    free(date);
    shutdown(socket_fd,SHUT_RDWR);

    for (size_t i = 0; i < file_count; i++)
    {
        free(*(files+i));
    }
    free(files);

    
    close(socket_fd);
}


struct http_request_requestLine parseRequest(char* request)
{
    
    struct http_request_requestLine req;
    req.method = malloc(256);
    req.HTTP_version = malloc(256);
    req.request_URI = malloc(256);
    
    int flag = 0;
    int finish = 0;
    char* temp_request = request;
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

char* generateResponse(struct http_request_requestLine request,char** files,int file_count,char* date)
{   
    
    char* response = malloc(500);
    char** temp_files = files;

    for (int i = 0; i < file_count; i++)
    {
        if(strcmp(request.request_URI+1,*(temp_files+i)) == 0) // * +1 for request.request_URI is to remove "/"
        {
            // printf("[\e[1;32m200 OK\e[0m] %s\n",*(temp_files+i));
            char* response_body = malloc(1000);
            int file_dp;
            char* file_path = malloc(50);
            sprintf(file_path,"/home/maqa/C-Http-server/resources/%s",request.request_URI+1);
            file_dp = open(file_path, O_RDONLY);
            printf("%d bytes were read\n",read(file_dp,response_body,BUFF_SIZE));
            sprintf(response,"%s 200 OK\r\n%s\n\n%s",request.HTTP_version,date,response_body);

            free(response_body);
            free(file_path);
            close(file_dp);

            return response;
        }
        // printf("\e[32m comparing  %s --  %s\e[0m\n",request.request_URI+1,*(temp_files+i));
        // printf("%i\n",strcmp(request.request_URI+1,*(temp_files+i))); // * +1 for request.request_URI is to remove "/"
    }

    // printf("[\e[1;31m404 NOT FOUND\e[0m] %s\n",request.request_URI+1);
    
    char* response_body = malloc(1000);
    int file_dp;
    file_dp = open("/home/maqa/C-Http-server/resources/404.html", O_RDONLY);
    printf("%d bytes were read\n",read(file_dp,response_body,BUFF_SIZE));
    sprintf(response,"%s 404 NOT FOUND\r\n%s\n\n%s",request.HTTP_version,date,response_body);
    

    free(response_body);
    close(file_dp);

    return response;
    
    // for (int i = 1; i < file_count; i++)
    // {
    //     free(*(files+i));  //! TODO: FREE MEMORY OF THE FILE NAMES !!!
    // }
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

struct http_request parseRequest_TEST(char* request)
{

    struct http_request ht_request;
    // ht_request.Host = malloc(50);

    char* request_temp = request;
    int cnt = 0;
    int length = 0;
    // char* test = malloc(100);
    while (*request_temp != '\0' )
    {
        if(*(request_temp+1) == '\n' && *(request_temp) == '\r')
        {
            request_temp++;
            request_temp++;
            if(strncmp("Connection",request_temp,10) == 0 )
            {
                // printf("%c--%c--%c\n",*(request_temp+2),*(request_temp+3),*(request_temp+4));
                for (int i = 0; i < 12; i++)
                request_temp++;

                while( *(request_temp+1) != '\r')
                {
                    request_temp++; 
                    length++;
                }
                
                ht_request.Connection = malloc(length);
                strncpy(ht_request.Connection,request_temp-length,length+1); //* be careful for +1 
                // printf("%s\n",ht_request.Connection);
                // printf("Connection %d\n",length);
                length = 0;
            }
            if (strncmp("Host",request_temp,4) == 0)
            {
                // printf("%c--%c--%c\n",*(request_temp),*(request_temp+1),*(request_temp+2));
                for (int i = 0; i < 6; i++)
                request_temp++;

                while(*(request_temp+1) != '\r')
                {
                    request_temp++;
                    length++;

                }

                ht_request.Host = malloc(length);
                strncpy(ht_request.Host,request_temp-length,length+1); //* be careful for +1 
                
                // printf("%s\n",ht_request.Host);
                // printf("Host %d\n",length);
                length = 0;
            }
            // else
            // {
                // printf("DALDAN\n");
            // }
            // printf("%c--%c--%c\n",*(request_temp+2),*(request_temp+3),*(request_temp+4));

        }
        // printf("%c",*request_temp);
        request_temp++;
    }
    // printf("%s\n",test);
    // printf("%i\n",cnt);
    return ht_request;
    
}

