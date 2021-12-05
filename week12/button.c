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
#include <linux/input.h>
#include "button.h"

#define INPUT_DEVICE_LIST "/dev/input/event"
#define PROBE_FILE "/proc/bus/input/devices"

#define HAVE_TO_FIND_1 "N: Name=\"ecube-button\"\n"
#define HAVE_TO_FIND_2 "H: Handlers=kbd event"


pthread_t button_t;


static int fp;
static int readSize, inputIndex;
struct input_event stEvent;
static char inputDevPath[200]={0,};
static int msgID;
static int status;

int probeButtonPath(char *newPath)
{
	int returnValue = 0;
	int number = 0;
	
	FILE *fp = fopen(PROBE_FILE, "rt");
	while(!feof(fp))
	{
		char tmpStr[200];
		fgets(tmpStr,200,fp);
		if(strcmp(tmpStr,HAVE_TO_FIND_1)==0)
		{
			returnValue = 1;
		}
		if((returnValue == 1) && (strncasecmp(tmpStr, HAVE_TO_FIND_2,strlen(HAVE_TO_FIND_2)) == 0))
		{
			number = tmpStr[strlen(tmpStr)-3] - '0';
			break;
		}
	}
	fclose(fp);
	if(returnValue == 1)sprintf(newPath, "%s%d", INPUT_DEVICE_LIST,number);
	return returnValue;
}

void* button_read(void *arg)
{
	BUTTON_MSG_T sendButton;
	
	msgID = msgget ((key_t)MESSAGE_ID, IPC_CREAT|0666);
	if(msgID == -1)
	{
		printf("msgget failed!\n");
		return 0;
	}
	
	while(1)
	{
		readSize = read(fp, &stEvent, sizeof(stEvent));
		if(readSize != sizeof(stEvent))continue;
		if(stEvent.type == EV_KEY)
		{
			switch(stEvent.code)
			{
				case KEY_VOLUMEUP: 
				sendButton.messageNum = 1;
				sendButton.keyInput = 1;
				
				break;
				
				case KEY_HOME: 
				sendButton.messageNum = 1;
				sendButton.keyInput = 2;

				break;
				
				case KEY_SEARCH: 
				sendButton.messageNum = 1;
				sendButton.keyInput = 3;

				break;
				
				case KEY_BACK: 
				sendButton.messageNum = 1;
				sendButton.keyInput = 4;

				break;
				
				case KEY_MENU: 
				sendButton.messageNum = 1;
				sendButton.keyInput = 5;

				break;
				
				case KEY_VOLUMEDOWN: 
				sendButton.messageNum = 1;
				sendButton.keyInput = 6;

				break;
			}
			
			if(stEvent.value) 
			{
				sendButton.pressed = 1;
				msgsnd(msgID, &sendButton, sizeof(sendButton.pressed)+sizeof(sendButton.keyInput), 0);
			}
			else 
			{
				sendButton.pressed = 0;
				msgsnd(msgID, &sendButton, sizeof(sendButton.pressed)+sizeof(sendButton.keyInput), 0);
			}
			usleep(1);
			msgrcv(msgID, &sendButton, sizeof(sendButton.pressed)+sizeof(sendButton.keyInput), 0,IPC_NOWAIT);
			if(sendButton.keyInput == 10)break;
		}
	}
}

int buttonInit(void)
{
	int status = 0;
	if(probeButtonPath(inputDevPath) == 0)
	{
		printf("fp Error!\n");
		return 0;
	}

	fp = open(inputDevPath, O_RDONLY);
	
	if(fp < 0)printf("open failed!\n");
	
	if((status = pthread_create(&button_t, NULL, &button_read, NULL)) != 0)
	{
        printf("Button_t failed!\n");
        return 0;
    }

    return 1;
}

int buttonExit(void)
{
	close(fp);
	pthread_join(button_t, NULL);
}
