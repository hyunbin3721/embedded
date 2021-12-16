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
//#include <sys/stat.h>
//#include <sys/mman.h>
//#include <mad.h>
//#include <pulse/simple.h>
//#include <pulse/error.h>
//#include <alsa/asoundlib.h>
//#include <alsa/mixer.h>
//#include <
//#include <
#include "FND.h"
#include "led.h"
#include "button.h"
#include "buzzer.h"
#include "colorled.h"
#include "temperature.h"
#include "textlcd.h"
#include "accelMagGyro.h"

#define MP3_PATH "/home/ecube/mp3/"

static pthread_t player;
static int fd  = 0;
static int msgID;
static int status = 0;
static int LEDstatus = 0;
static int VOLstatus;
static int textLcdstatus = 0;

BUTTON_MSG_T recieveButton;

int ledMid(void);
int VOLUP(void);
int VOLDOWN(void);
int HOME(void);
int SEARCH(void);
int BACK(void);
int MENU(void);

int main(void)
{
	
	ledLibInit();ledMid();//LED시작, LED1,2,3,4,5 ON
	pwmLedInit();
	textlcdInit();
	FND_init();
	
	if(buttonInit() != 1)printf("buttonInit Failed!\n");
	
	fd = open(MP3_PATH "/list.txt", O_RDONLY); //곡 리스트 Open
	if(fd < 0)
	{
		printf("list open error!\n");
		exit(-1);
	}
	
	while(1)//버튼입력 시작.
    {
		msgrcv(msgID, &recieveButton, sizeof(recieveButton.pressed)+sizeof(recieveButton.keyInput), 0,0);
		if(recieveButton.pressed)printf("pressed.\n");
		
		else 
		{
			printf("released\n");
			switch(recieveButton.keyInput)
			{
				case 1:
						VOLUP();//음량 높임
						break;
					 
				case 2: 
						HOME();//첫 번째 곡으로 이동
						break;
					 
				case 3:
						SEARCH();//다음 곡
						break;
					 
				case 4:
						BACK();//이전 곡
						break;
					 
				case 5:
						MENU();//재생, 정지
						break;
			
				case 6:
						VOLDOWN();//음량 낮춤
						break;
			}
		}
		//if(status == 1)break;
	}
	
	
	buttonExit();
}

int ledMid(void)
{
	for(int ledCnt = 0; ledCnt < 4; ledCnt++)
	{
		ledOnOff(ledCnt,1);
	}
	
	LEDstatus = 4;
}

int VOLUP(void)
{
	ledOnOff(LEDstatus,1);
	LEDstatus++;
	if(LEDstatus > 7)LEDstatus = 7;
}

int VOLDOWN(void)
{
	ledOnOff(LEDstatus,0);
	LEDstatus--;
	if(LEDstatus < 0)LEDstatus = 0;
}

int HOME(void)
{
	printf("to first music or last\n");
	//pthread로 mp3 디코더 호출
	
}

int MENU(void)
{
	printf("song should played button\n");
	//pthread로 mp3 디코더 호출
	
}

int BACK(void)
{
		lcdtextwrite("song name back", " ", 1);
				
}

int SEARCH(void)
{
	lcdtextwrite("song name search", " ", 1);
}
