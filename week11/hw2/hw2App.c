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

#define devName "/dev/hw2"
#define devNum 400
#define MagicNum 0x55

int main(int argc, char **argv)
{
    char data[5]={'1','2','3','4','5'};
    int N = sizeof(data);
    int fd;
    printf("enter test\n");

    fd = open(devName, O_RDWR);
    if(fd < 0)
    {
        printf("open fail\n");
        perror("driver (/dev/test_dev) open error\n");
        return 1;
  }
    printf("open success\n");

	
    write(fd, &data, N);
    read(fd, data, N);
    printf("read value:");
    for(int a=0; a<N; a++)
    {
		printf("%c",data[a]);
	}
	printf("\n");
	

    for(int k=0; k<8; k++)
    {
        int request = 1<<k;
        unsigned int inputCmd = _IO(MagicNum, request);
        printf("Enter CMD:%d\n",inputCmd);
        unsigned long returnValue = ioctl(fd, inputCmd, 0);
    }
    char array[20];
	unsigned int inputCmd =_IOW(MagicNum, 99, array);
    unsigned long returnValue = ioctl(fd, inputCmd, array);
    
    inputCmd = _IOW(MagicNum, 98, int);
    int count =5;
    returnValue = ioctl(fd, inputCmd, &count);
    
    inputCmd = _IOW(MagicNum, 97, int);
    returnValue = ioctl(fd, inputCmd, &N);
    
    close(fd);
    
    return 0;
}
