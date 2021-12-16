#include "FND.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DRIVER_PATH "/dev/perifnd"

static int fd;
static inputstruct buffer;

int FND_init(void)
{
	if((fd = open(DRIVER_PATH, O_WRONLY)) < 0)
	{
		printf("Driver open failed!\n");  
		exit(-1);
	}
	else return 0;
	
	FND_write(0, 0);
	if(FND_on() == 1)
	{
		printf("Driver writing failed!\n");
		exit(-1);
	}
}

int FND_write(unsigned int numInput, unsigned int dotInput)
{
	unsigned char FND[2][6] = {
		{0,},
		{0,}
	};	
	int cnt = 0;
	int dev = 100000; int dev2 = 10;
	
	for(cnt = 0; cnt < 6; cnt++)
	{
		
		if(cnt == 5) 
		{ 
			FND[0][cnt] = numInput % 10;
			FND[1][cnt] = dotInput;
		}
		else
		{
			FND[0][cnt] = numInput / dev ;
			FND[1][cnt] = dotInput % dev2;
			numInput = numInput - FND[0][cnt]*dev;
			dotInput = dotInput / dev2;
			dev = dev / 10;
		}
		
		if((FND[1][cnt] != 1) && (FND[1][cnt] !=0)) return 2;
		 
		buffer.DataNumber[cnt] = FND[0][cnt];
		buffer.DataDot[5-cnt] = FND[1][cnt];
	}
	
	
	if(write(fd, &buffer, sizeof(buffer)) < 0)
	{
		return 1;
	}
	FND_clean();
	return 0;
	
}

int FND_on_clock(void)
{
	int cnt = 0;
	for(cnt = 1; cnt < 5; cnt++)
	{
		buffer.DataPower[cnt] = 1;
	}
	buffer.DataPower[0] = 0;
	buffer.DataPower[0] = 0;
	buffer.DataDot[2] = 1;
	
	if(write(fd, &buffer, sizeof(buffer)) < 0)
	{
		return 1;
	}
	FND_clean();
	return 0;
}
int FND_on(void)
{
	int cnt = 0;
	for(cnt = 0; cnt < 6; cnt++)
	{
		buffer.DataPower[cnt] = 1;
	}
	if(write(fd, &buffer, sizeof(buffer)) < 0)
	{
		return 1;
	}
	else return 0;
}
int FND_clean(void)
{
	int cnt = 0;
	for(cnt = 0; cnt < 6; cnt++)
	{
		buffer.DataDot[cnt] = 0;
		buffer.DataNumber[cnt] = 0;
	}
}

int FND_off(void)
{
	int cnt = 0;
	
	for(cnt = 0; cnt < 6; cnt++)
	{
		buffer.DataNumber[cnt] = 0;
		buffer.DataDot[cnt] = 0;
		buffer.DataPower[cnt] = 0;
	}
	
	if(write(fd, &buffer, sizeof(buffer)) < 0)
	{
		return 1;
	}
	else return 0;
}

int FND_exit(void)
{
	FND_off();
	if(close(fd) != 0)
	{
		printf("Driver close failed!\n");
		exit(-1);
	}
}
