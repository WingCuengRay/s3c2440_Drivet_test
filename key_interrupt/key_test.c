#include <stdio.h>
#include <signal.h>
#include <sys/resource.h>

int main(int argc,char **argv)
{
    int fd;
    int i;
    int press_cnt[4] = {0};

    fd = open("/dev/key", 0);
    if(fd < 0)
    {
        printf("Cannot open /dev/key!\n"); 
        return -1;
    }

    while(1)
    {
        read(fd, press_cnt, 4);
        for(i=0; i<4; i++)
            printf("%d key has been pressed %d times.\n", i+1, press_cnt[i]);
    }
        
   return 0;
}
