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

#define LED_DRIVER_NAME     "/dev/periled"

int main(void)
{
    int ledNum = 0;
    int ledStat = 0;
    int i=0; 

    ledLibInit();

	for (i=0;i<8;i++)
	{
		ledOnOff(i, 1);		// i번째 led 키기
		sleep(1);
		ledStat = ledStatus();
		printf("led Current Status: %02x\n", ledStat);	
		ledOnOff(i,0);		// i번째 led 끄기
	}

    sleep(1);
	printf("All led On\n");
	ledifAns();
    ledLibExit();
    return 0;
}