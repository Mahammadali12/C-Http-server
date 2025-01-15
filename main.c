#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    // struct sockaddr_in address;

    // address.sin_family = AF_INET6;
    // address.sin_port = htons(8080);

    pid_t pid;
    printf("Fork here. %d \n",pid);
    
    pid = fork();
    if (pid == -1)
    {
        // If fork returns -1, there was an error!
        return (1);
    }
    printf("\nFork successful!\n");
    

    if (pid == 0)
    {
        // Fork's return value is 0:
        // we are now in the child process
        printf("Child: I'm the child, my internal pid is %d.\n", pid);
        sleep(1);
        printf("Hello, I am awake from the sleep\n");

    }
    else if (pid > 0)
    {
        // Fork's return value is not 0
        // which means we are in the parent process
        printf("Parent: I'm the parent, my child's pid is %d.\n", pid);
    }
    return(0);

}