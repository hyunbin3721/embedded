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
#include <pthread.h>
#include <sys/msg.h>
#include "button.h"

BUTTON_MSG_T recieveButton;

int main(void)
{
	int msgID;
	int status = 0;

	
	if((msgID = msgget((key_t)MESSAGE_ID, IPC_CREAT|0666)) != 0)
	{
        printf("msgget failed!\n");
        exit(-1);
    }
    
    if(buttonInit() != 1)printf("buttonInit Failed!\n");
	
    while(1)
    {
		msgrcv(msgID, &recieveButton, sizeof(recieveButton.pressed)+sizeof(recieveButton.keyInput), 0,0);
		
		if(recieveButton.pressed)printf("pressed.\n");
		else 
		{
			switch(recieveButton.keyInput)
			{
				case 1: printf("VOLUP "); break;
				case 2: printf("HOME "); break;
				case 3: printf("SEARCH "); break;
				case 4: printf("BACK "); break;
				case 5: printf("MENU "); break;
				case 6: printf("VOLDOWN "); break;
			}
			printf("released\n");
		}
		if(status == 1)break;
	}
	
	buttonExit();
	return 0;
}
    
    
	
