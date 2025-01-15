#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char* filename = "/home/maqa/C-Http-server/daldan_durden";
    int file_dp;
    file_dp = open (filename,O_CREAT  | O_APPEND | O_WRONLY, 0640);

    unlink(filename);

    close(file_dp);

}