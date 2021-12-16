#include "FND.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static char inputBuffer[200];
static unsigned int number;
static unsigned int dot;

static char *tmp;

int main(void)
{
	int status;
	FND_init();
	
	while(1)
	{
		printf("Input please!: ");
		fgets(inputBuffer,sizeof(inputBuffer),stdin);
		inputBuffer[strlen(inputBuffer)-1] = '\0';
		
		if(strcmp(inputBuffer,"--help") == 0)
		{
			printf("==================================================\n");
			printf("number, dot(use 1 or 0 to ON/OFF)\n");
			printf("on for FND power on.\n");
			printf("off for FND power off.\n");			
			printf("exit for termination.\n");
			printf("==================================================\n");
			continue;
		}
		
		else if(strcmp(inputBuffer,"on") == 0)
		{
			if(FND_on() != 0)printf("FND on failed!\n");
			continue;
		}
		
		else if(strcmp(inputBuffer,"off") == 0)
		{
			if(FND_off()==1)printf("Driver writing failed!\n");
			continue;
		}
		
		else if(strcmp(inputBuffer,"exit") == 0)
		{
			break;
		}
	
		else
		{
			tmp = strtok(inputBuffer,", ");
			
			number = atoi(tmp);
			if((number > 999999) || (number < 0))
			{
				printf("Number error! --help for instruction!\n");
				continue;
			} 
			
			tmp = strtok(NULL," ,\0");		
			if(tmp == NULL)
			{
				printf("Input error! --help for instruction!\n");
				continue;
			} 
			
			dot = atoi(tmp);
			if((dot > 999999) || (dot < 0))
			{
				printf("Number error! --help for instruction!\n");
				continue;
			} 
			
			tmp = strtok(NULL," ,\0");
			if(tmp != NULL)printf("Any input after second will be ignored!\n");
			
			status = FND_write(number, dot);
			
			if(status == 1)
			{
				printf("Driver writing failed!\n");
				continue;
			}
			
			if(status == 2)
			{
				printf("Dot error! --help for instruction!\n");
				continue;
			}
		}
	}
	
	FND_exit();
	printf("Goodbye!\n");
	
	return 0;
}
