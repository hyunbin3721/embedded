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

int main(void)
{
	int status = 0;
	int msgID;
	BUTTON_MSG_T recieveButton;

	printf("init\n");
	if((status = msgget((key_t)MESSAGE_ID, IPC_CREAT|0666)) != 0)
	{
        printf("msgget failed!\n");
        exit(-1);
    }
    printf("init\n");
    if(buttonInit() != 1)printf("buttonInit Failed!\n");exit(-1);
	printf("init\n");
    
    while(1)
    {
		printf("test\n");
		msgrcv(msgID, &recieveButton, sizeof(recieveButton.pressed)+sizeof(recieveButton.keyInput), 0,0);
		switch(recieveButton.keyInput)
		{
			case 1: printf("VOLUP "); break;
			case 2: printf("HOME "); break;
			case 3: printf("SEARCH "); break;
			case 4: printf("BACK "); break;
			case 5: printf("MENU "); break;
			case 6: printf("VOLDOWN "); break;
		}
		
		if(recieveButton.pressed)printf("pressed.\n");
		else printf("released\n");
	}
}
    
    
	
