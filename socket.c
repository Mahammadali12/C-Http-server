#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main ()
{

    struct addrinfo* res;
    struct addrinfo hints;

    char buffer [1000];


    getaddrinfo("www.ada.edu.az", "http",&hints,&res); // converts url to ip address and stores it in res->ai_addr
    
    
    // printf("%s\n",res->ai_addr->sa_data);
    // printf("%s\n",res->ai_addr->sa_family);



    struct sockaddr_in* ipv4 = (struct sockaddr_in *) res->ai_addr;  //converts general ip to ipv4 
    

    // printf("%s\n",ipv4->sin_addr);
    // ipv4->sin_addr = INADDR_LOOPBACK;

    inet_ntop(res->ai_family, &(ipv4->sin_addr), buffer, sizeof buffer);  

    printf("%s\n",buffer);
    // char* url = "www.example.com";

    // printf("%d\n",);
    
    
}
