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
#include "led.h"

static unsigned int ledValue = 0;
static int fd =0;

int ledOnOff (int ledNum, int onOff)
{
	int written;
	int i=1;
	i = i<<ledNum;
	ledValue = ledValue&(~i);
	if (onOff != 0) ledValue |= i;
	written = write (fd, &ledValue, 4);
	return written;
}

int ledLibInit(void)
{
	fd = open("/dev/periled", O_WRONLY);
	ledValue = 0;
	return fd;
}

int ledLibExit(void)
{
	ledValue = 0;
	ledOnOff (0,0);
	close(fd);
	return 0;
}

int ledStatus(void)
{
	return ledValue;
}

int ledifAns(void)
{
	int i, k;
	
	for(k=0;k<3;k++){
		for(i=0;i<8;i++){
			ledOnOff(i, 1);
		}
		usleep(500000);
		for(i=0;i<8;i++){
			ledOnOff(i, 0);
		}
		usleep(500000);
	}

	return 0;
}