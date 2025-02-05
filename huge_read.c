#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>

int main(void)
{
    int file_dp = open("/home/maqa/C-Http-server/resources/test.txt", O_RDONLY);
    int bytes_read = 0;
    char* buffer = malloc(512);

    // printf("\e[1m%d\e[0m bytes were read\n",read(file_dp,buffer,512));

    // printf("%s\n",buffer);

    // printf("--------\n");

    // free(buffer);

    // buffer = malloc(512);
    // printf("\e[1m%d\e[0m bytes were read\n",read(file_dp,buffer,512));

    // printf("%s\n",buffer);

    while ((bytes_read = read(file_dp,buffer,512)) != 0)
    {
        
        printf("\e[1m%d\e[0m bytes were read\n",bytes_read);

        printf("%s\n",buffer);

        printf("--------\n");

        free(buffer);

        buffer = malloc(512);

        sleep(5);
    }
    
    free(buffer);


}