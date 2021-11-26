#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define devName "/dev/hw1"
#define devNum 300

int main(int argc, char **argv)
{
    char data[4] = {'1','2','3','4'};
    int fd;
    printf("enter test\n");

    fd = open(devName, O_RDWR);
    if(fd < 0)
    {
        printf("open fail\n");
        perror("driver (/dev/hw1) open error\n");
        return 1;
    }
    printf("open success\n");


    write(fd, &data, 4);
    read(fd, data, 4);
    printf("read value:%c%c%c%c\n", data[0],data[1],data[2],data[3]);

    close(fd);
    
    return 0;
}
