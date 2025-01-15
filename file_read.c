#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>

int main ()
{
    int file_dp;
    char* file_path = "/home/maqa/C-Http-server/main.c";
    file_dp = open(file_path,O_RDONLY);

    char* buffer = (char*) malloc(2881);

    printf("%d\n", read(file_dp,buffer,2879));
    printf("%s",buffer);

}