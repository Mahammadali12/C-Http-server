#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char* filename = "/home/maqa/C-Http-server/daldan_durden\n";
    int file_dp;
    file_dp = open (filename,O_CREAT  | O_APPEND | O_WRONLY, 0640);


    
    while (write(file_dp,"DALDAN\n",7)>0)
    {
        printf("%d\n",file_dp);
    }
    
    

    close(file_dp);

}