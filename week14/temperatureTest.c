#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "temperature.h"

int main(void)
{
	double thermal; int cnt;
	if(temp_init() != 1)
	{
		printf("thermal_init error!\n");
		exit(-1);
	}
	printf("hello! I'll measure and show Temperature for 10 seconds!\n\n");
	
	for(cnt = 0; cnt < 10; cnt++)
	{
		thermal = temp_read();
		
		printf("Temperature is: %f\n", thermal);
		
		sleep(1);
	}
	
	printf("\n");
	
	if(temp_exit() != 0)
	{
		printf("exit error!\n");
		exit(-1);
	}
	
	printf("goodbye!\n");
	
	return 0;
}
