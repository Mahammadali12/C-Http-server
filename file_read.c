#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>

int main ()
{
    int file_dp;
    char* file_path = "/home/maqa/C-Http-server/index.html";
    file_dp = open(file_path,O_RDONLY);

    char* buffer = (char*) malloc(2881);

    printf("%d\n", read(file_dp,buffer,211));
    printf("%s",buffer);

}