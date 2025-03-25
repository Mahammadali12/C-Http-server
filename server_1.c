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
#define BUFF_SIZE 8192
#define L_BUFF_SIZE 10240

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
    char* Accept;
};

struct http_request_requestLine parseRequest(char* request);
void  generateResponseAndSendResponse(struct http_request_requestLine request,char** files,int file_count,char* date,int client_fd,struct http_request full_request);
char** getResources(int* file_count);
void getCurrentTime(char* d);
void parseRequest_TEST(char* request,struct http_request * ht);




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

    int client_fd;
    while (1)
    {

        struct sockaddr_storage dd;
        socklen_t addrlen;
        if ((client_fd = accept(socket_fd,(struct sockaddr*)&addr,&addrlen)) == -1)
        {
            perror("ACCEPT()");
            return(1);
        }

        printf("[\e[1;32mCLIENT\e[0m] %d\n[\e[1;32mSERVER\e[0m] %d\n",client_fd,socket_fd);

        char* recieved_msg = malloc(2048);
        int received_byte;
        // int total_received_byte = 0;

        // if((
            while(( 
                received_byte = recv(client_fd,recieved_msg,2048,0)
                ) != -1)   
            {
                printf("\e[1m%d\e[0m bytes were received from client: \e[1m%d\e[0m\n",received_byte,client_fd);
                printf("\e[33m%s\e[0m\n",recieved_msg);

                printf("%d\n",strlen(recieved_msg));

                // for(int i = 0 ; i< strlen(recieved_msg) ;i++)
                // {
                //     if (*(recieved_msg+i) != '\n' && *(recieved_msg+i) != '\r' && *(recieved_msg+i) != '\0')
                //     {
                //         printf("%d ",*(recieved_msg+i));
                //     }
                //     else if (*(recieved_msg+i) == '\n')
                //     {
                //         printf("%c",*(recieved_msg+i));
                //         if(*(recieved_msg +i+1) == '\r')
                //         {
                //             printf("\e[31msecond carriage was found exiting program\e[0m\n");
                //             break;
                //         }                    }
                //     else if (*(recieved_msg + i) == '\r')
                //     {
                //         printf("carriage was found");

                //     }
                //     else if (*(recieved_msg+i) == '\0')
                //     {
                //         printf("back slash was found");
                //     }                    
                // }
                

                struct http_request_requestLine request = parseRequest(recieved_msg);

                struct http_request rq;
                parseRequest_TEST(recieved_msg,&rq);
                generateResponseAndSendResponse(request,files,file_count,date,client_fd,rq);
                // int sent_byte = 0;
                // if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
                // {
                //     perror("ZERO BYTES WERE SENT");
                //     return(1);
                // }


                printf("--------------\n");

                printf("--------------\n");

                printf("Connection: %s %d\n",rq.Connection,strlen(rq.Connection));
                printf("Host: %s %d\n",rq.Host,strlen(rq.Host));
                printf("Accept: %s %d\n",rq.Accept,strlen(rq.Accept));
                // printf("DASAS");

                // for(int i = 0; i < 31; i++)
                // {
                //     printf("DASAS");
                //     // printf("%c ",*(rq.Host+i));

                // }


                // for (int i = 0; i < strlen(rq.Connection); i++)
                // {
                //     printf("%d ",*(rq.Connection+i));
                // }
                // printf("\n");

                // for(int i = 0; i < strlen(rq.Host); i++)
                

                if(strcmp(rq.Connection,"close") == 0)
                break;
                // close(file_dp);
                close(client_fd);
                free(recieved_msg);

                // free(response);

                free(rq.Connection);
                free(rq.Host);
                free(rq.Accept);

                free(request.request_URI);
                free(request.method);
                free(request.HTTP_version);



                // free(recieved_msg);
                // recieved_msg = malloc(512);
            }
            // ) == 0)
        // if( received_byte == 0)
        // {       
        //     perror("ZERO BYTES WERE RECEIVED");
        //     return(1);
        // }

        // printf("%d bytes were received from client:%d\n",received_byte,client_fd);
        // printf("MESSAGE:\n");
        // printf("%s\n",recieved_msg);

        // char* response;
        // int file_dp;
        // file_dp = open("/home/maqa/C-Http-server/resources/index.html", O_RDONLY);
        // printf("%d bytes were read\n",read(file_dp,response,BUFF_SIZE));


        // struct generateResponseAndSendResponse(request,files,file_count,date,client_fd);
        // int sent_byte = 0;
        // if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
        // {
        //     perror("ZERO BYTES WERE SENT");
        //     return(1);
        // }

        // printf("%d bytes were sent as a response to the client(%d)\n",sent_byte,client_fd);
        // printf("Response:\n");
        // printf("%s",response);

        // printf("--------------\n");

        // printf("--------------\n");
        // struct http_request rq;
        // parseRequest_TEST(recieved_msg,&rq);
        // printf("Connection: %s %d\n",rq.Connection,strlen(rq.Connection));
        // for (int i = 0; i < strlen(rq.Connection); i++)
        // {
        //     printf("%d ",*(rq.Connection+i));
        // }
        // printf("\n");

        // printf("Host: %s \n",rq.Host);
        

        // for(int i = 0; i < 5; i++)
        // {
        //     printf("DASAS");
        //     printf("%c ",*(rq.Host));
            
        // }
                


        // if(strcmp(rq.Connection,"close") == 0)
        // break;
        // // close(file_dp);
        // close(client_fd);

        // free(recieved_msg);

        // free(response);

        // free(rq.Connection);
        // free(rq.Host);

        // free(request.request_URI);
        // free(request.method);
        // free(request.HTTP_version);
    }


    free(date);
    shutdown(socket_fd,SHUT_RDWR);

    for (size_t i = 0; i < file_count; i++)
    {
        free(*(files+i));
    }
    free(files);

    // close(client_fd);    
    close(socket_fd);
}


struct http_request_requestLine parseRequest(char* request)
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

void generateResponseAndSendResponse(struct http_request_requestLine request,char** files,int file_count,char* date,int client_fd,struct http_request full_request)
{     // ! TODO: REMEBER YOU CHANGED RETURN VALUE TO THE VOID
    
    char* response = malloc(L_BUFF_SIZE);
    // char** temp_files = files;

    for (int i = 0; i < file_count; i++)
    {
        if(strcmp(request.request_URI+1,*(files+i)) == 0) // * +1 for request.request_URI is to remove "/"
        {
            // printf("[\e[1;32m200 OK\e[0m] %s\n",*(temp_files+i));
            char* response_body = malloc(BUFF_SIZE);
            int file_dp;
            char* file_path = malloc(70);

            sprintf(file_path,"/home/maqa/C-Http-server/resources/%s",request.request_URI+1);

            file_dp = open(file_path, O_RDONLY);

            // printf("%d bytes were read\n",read(file_dp,response_body,BUFF_SIZE));

            int bytes_read = read(file_dp,response_body,BUFF_SIZE);
            printf("\e[1m%d\e[0m bytes were read to \e[31mresponse body\e[0m\n",bytes_read);
            char* content_type = malloc(50);
            sprintf(content_type,"Content-Type: %s",full_request.Accept);
            // strcpy(content_type,"Content-Type: %s");

            sprintf(response,"%s 200 OK\r\n%s\n%s\n\n%s",request.HTTP_version,content_type,date,response_body);


            int sent_byte = 0;
            if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
            {
                perror("ZERO BYTES WERE SENT");
            }

            printf("\e[1m%d\e[0m bytes were sent to \e[32mresponse\e[0m\n",sent_byte);
            printf("\e[33m%s\e[0m\n",response);
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
                printf("\e[33m%s\e[0m\n",response);

                sleep(1);
                free(response_body);
                free(response);
                response_body = (char*) malloc(BUFF_SIZE);
                response = (char*) malloc(L_BUFF_SIZE);
            }

            free(response_body);
            free(file_path);
            close(file_dp);
            return;
        }
        // printf("\e[32m comparing  %s --  %s\e[0m\n",request.request_URI+1,*(temp_files+i));
        // printf("%i\n",strcmp(request.request_URI+1,*(temp_files+i))); // * +1 for request.request_URI is to remove "/"
    }

    // printf("[\e[1;31m404 NOT FOUND\e[0m] %s\n",request.request_URI+1);
    
    char* response_body = malloc(L_BUFF_SIZE);
    int file_dp;
    file_dp = open("/home/maqa/C-Http-server/resources/404.html", O_RDONLY);

    int bytes_read = read(file_dp,response_body,BUFF_SIZE);
    printf("\e[1m%d\e[0m bytes were read to \e[31mresponse body\e[0m\n",bytes_read);

    // sprintf(response,"%s 200 OK\r\n%s\n\n%s",request.HTTP_version,date,response_body);
    char* content_type = malloc(70);
    sprintf(content_type,"Content-Type: %s", full_request.Accept);
    // strcpy(content_type,"Content-Type: text/html");

    sprintf(response,"%s 404 NOT FOUND\r\n%s\n%s\n\n%s",request.HTTP_version,content_type,date,response_body);

    int sent_byte = 0;
    if((sent_byte = send(client_fd,response,strlen(response),0)) == 0)
    {
        perror("ZERO BYTES WERE SENT");
    }

    printf("\e[1m%d\e[0m bytes were sent to \e[32mresponse\e[0m\n",sent_byte);
    printf("\e[33m%s\e[0m\n",response);
    printf("--------\n");
            // sleep(2);
    free(response_body);
    free(response);
    response_body = malloc(BUFF_SIZE);
    response = malloc(L_BUFF_SIZE);

        while ( (bytes_read = read(file_dp,response_body,BUFF_SIZE)) != 0)
        {
            printf("\e[1m%d\e[0m bytes were read to \e[31mresponse body\e[0m\n",bytes_read);
            sprintf(response,"%s",response_body);



            if((sent_byte = send(client_fd,response,bytes_read,0)) == 0)
            {
                perror("ZERO BYTES WERE SENT");
                return;
            }
            printf("\e[33m%s\e[0m\n",response);
            printf("\e[1m%d\e[0m bytes were sent to \e[32mresponse\e[0m\n",sent_byte);

            // sleep(1);
            free(response_body);
            free(response);
            response_body = malloc(BUFF_SIZE);
            response = malloc(L_BUFF_SIZE);
        }

    free(response);
    free(response_body);
    close(file_dp);

    // return response;
    
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

void parseRequest_TEST(char* request,struct http_request * ht)
{

    // struct http_request ht_request;
    
    // ht_request.Connection = malloc(4);
    // strcpy(ht_request.Connection,"NULL");
    // ht_request.Host = malloc(50);

    char* request_temp = request;
    int i=0;
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
                // free(ht_request.Connection);
                // printf("%c--%c--%c\n",*(request_temp+2),*(request_temp+3),*(request_temp+4));
                for (int i = 0; i < 12; i++)
                request_temp++;

                while( *(request_temp+1) != '\r')
                {
                    request_temp++; 
                    length++;
                }
                ht->Connection = malloc(length+2);
                strncpy(ht->Connection,request_temp-length,length+1); //* be careful for +1 
                *(ht->Connection+length+1) = '\0';
                // printf("%s\n",ht_request.Connection);
                // printf("Connection %d\n",length);
                length = 0;
                printf("\e[1m Connection was parsed\e[0m\n");
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

                ht->Host = malloc(length+2);
                strncpy(ht->Host,request_temp-length,length+1); //* be careful for +1 
                *(ht->Host+length+2) = '\0';
                // printf("%s\n",ht_request.Host);
                // printf("Host %d\n",length);
                length = 0;
                printf("\e[1m Host was parsed\e[0m\n");
            }
            if(strncmp("Accept:", request_temp, 7) == 0 )
            {
                for (int i = 0; i < 8; i++)
                request_temp++;

                while (*(request_temp+1) != ',' && *(request_temp+1) != ';' && *(request_temp+1) != ',')
                {
                    request_temp++;
                    length++;
                }

                ht->Accept = malloc(length+2);
                strncpy(ht->Accept,request_temp-length,length+1);
                *(ht->Accept+length+1) = '\0';
                
                length = 0;
                printf("\e[1m Accept was parsed\e[0m\n");
            }
            // else
            // {
                // printf("DALDAN\n");
            // }
            // printf("%c--%c--%c\n",*(request_temp+2),*(request_temp+3),*(request_temp+4));
            
        }
        // printf("%c",*request_temp);
        request_temp++;
        // printf("%d\n",i++);
    }
    // printf("%s\n",test);
    // printf("%i\n",cnt);
    // return &ht_request;
    
}

