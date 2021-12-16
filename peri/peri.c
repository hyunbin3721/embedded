#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
/*
#include "./mad/mad.h"//실제 라이브러리 추가 필요
#include <pulse/simple.h>
#include <pulse/error.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
 */

#include "FND.h"
#include "led.h"
#include "button.h"
#include "buzzer.h"
#include "colorled.h"
#include "temperature.h"
#include "textlcd.h"
#include "accelMagGyro.h"

#define MP3_PATH "/home/ecube/mp3/"
#define MEMADD 1234//공유메모리 주소

//PWM----------------------------------------------------------
/*#define COLOR_LED_DEV_R_ "/sys/class/pwm/pwmchip0/"
#define COLOR_LED_DEV_G_ "/sys/class/pwm/pwmchip1/"
#define COLOR_LED_DEV_B_ "/sys/class/pwm/pwmchip2/"

#define PWM_EXPORT "export"
#define PWM_UNEXPORT "unexport"
#define PWM_DUTY "pwm0/duty_cycle"
#define PWM_PERIOD "pwm0/period"
#define PWM_ENABLE "pwm0/enable"

#define PWM_COLOR_R 0
#define PWM_COLOR_G 1
#define PWM_COLOR_B 2*/
//----------------------------------------------------------------------


static int msgID;
static int forkStatus;
static pid_t decoderPid, clockPid;

static int fd  = 0;
static char *tmpBuff;
static char fileBuf[2000];
char nameBuf[10][16];
static int listIndex = 0;
static int LEDstatus = 0;
static int VOLstatus;
static int textLcdstatus = 0;
static int homeStatus = 0;
static int menuStatus = 0;
static unsigned int tim = 0;
static int timCount = 0;
BUTTON_MSG_T recieveButton;


int ledMid(void);
int VOLUP(void);
int VOLDOWN(void);
int HOME(void);
int SEARCH(void);
int BACK(void);
int MENU(void);
int decode(int input, int color);
int FNDClock(int input);

int main(void)
{
	DIR *list;
	struct dirent *dir;
	int status;
	list = opendir(MP3_PATH);
	
	if(list == NULL)
	{
		printf("mp3 directory open failed!\n");
		exit(-1);
	}
	
	while((dir = readdir(list)))
	{
		if(dir->d_type == DT_REG)
		{
			strcpy(nameBuf[listIndex],dir->d_name);
			printf("buf: %s\n",nameBuf[listIndex]); 
			listIndex++;
		}
	}
	
	closedir(list);
	
	ledLibInit();ledMid();//LED시작, LED1,2,3,4,5 ON
	pwmLedInit();

	textlcdInit();textlcdclear();lcdtextwrite(nameBuf[0]," ", 1);
	FND_init();FND_on_clock();
	
	if(buttonInit() != 1)printf("buttonInit Failed!\n");
	
	decoderPid = clockPid = -1;
	
	decoderPid = fork();
	if(decoderPid < 0)
	{
		printf("decoderPid failed!\n");
		exit(-1);
	}
	
	if(decoderPid>0)clockPid = fork();
		
	if(decoderPid > 0 && clockPid > 0)//main
	{
		int shmID = shmget((key_t)MEMADD, 1, IPC_CREAT|0666);
		if(shmID == -1)
		{
			printf("shmget error!\n");
			exit(-1);
		}
		char *shmemAddr;
		shmemAddr = shmat(shmID, (void*)NULL, 0);
		if(((int)(shmemAddr))== -1)
		{
			printf("shmat error!\n");
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
							*((int*)shmemAddr) = MENU();//재생, 정지
							break;
			
					case 6:
							VOLDOWN();//음량 낮춤
							break;
				}
			}
		}
	
		waitpid(clockPid,&status,0);
		waitpid(decoderPid,&status,0);
				
		pwmInactiveAll();
		ledLibExit();
		buttonExit();
	}
	
	else if(decoderPid== 0 && clockPid == -1)//decoder PID
	{
		int shmID = shmget((key_t)MEMADD, 1, IPC_CREAT|0666);
		if(shmID == -1)
		{
			printf("shmget error!\n");
			exit(-1);
		}
		char *shmemAddr;
		shmemAddr = shmat(shmID, (void*)NULL, 0);
		if(((int)(shmemAddr))== -1)
		{
			printf("shmat error!\n");
		}
		
		while(1)
		{
			for(int i = 0; i < 3; i++)
			{
				decode(*((int*)shmemAddr), i);
			}
		}	//디코더 실행
	}
	
	else if(decoderPid > 0 && clockPid ==0)//clock PID;
	{
		printf("clock pid init!\n");
		
		int shmID = shmget((key_t)MEMADD, 1, IPC_CREAT|0666);
		if(shmID == -1)
		{
			printf("shmget error!\n");
			exit(-1);
		}
		char *shmemAddr;
		shmemAddr = shmat(shmID, (void*)NULL, 0);
		if(((int)(shmemAddr))== -1)
		{
			printf("shmat error!\n");
		}
		while(1)
		{
			FNDClock(*((int*)shmemAddr));
		}
	}
	
	else//fork 실패
	{
		printf("Fork failed!\n");
		exit(-1);
	}
}

int ledMid(void)
{
	for(int ledCnt = 0; ledCnt < 4; ledCnt++)
	{
		ledOnOff(ledCnt,1);
	}
	
	LEDstatus = 3;
}

int VOLUP(void)
{
	LEDstatus++;
	if(LEDstatus > 7)LEDstatus = 7;
	ledOnOff(LEDstatus,1);
}

int VOLDOWN(void)
{
	ledOnOff(LEDstatus,0);
	LEDstatus--;
	if(LEDstatus < 0)LEDstatus = 0;
}

int HOME(void)
{
	int maxIndex = sizeof(nameBuf[0]);
	
	if(homeStatus == 1)
	{
		lcdtextwrite(nameBuf[0], " ",1);
		homeStatus = 0;
	}
	else
	{
		lcdtextwrite(nameBuf[maxIndex], " ",1);
	}
	printf("to first music or last\n");
	
}

int MENU(void)
{
	printf("song should played button\n");
	if(menuStatus == 0)
	{
		menuStatus = 1;
		return 1;
	}
	else
	{
		menuStatus = 0;
		return 0;
	}
	
}

int BACK(void)
{
	listIndex--;
	if(listIndex < 0)listIndex = 0;
	lcdtextwrite(nameBuf[listIndex], " ", 1);			
}

int SEARCH(void)
{
	int maxIndex = sizeof(nameBuf[0]);
	listIndex++;
	if(listIndex > maxIndex)listIndex = maxIndex;
	lcdtextwrite(nameBuf[listIndex], " ", 1);
}

int decode(int input, int color)
{
	int playing = input;
	int i = color;
	
	if(playing)//디코딩,재생
	{
		printf("testing!\n");
		//for(int i=0; i<3; i++)
		//{   
			pwmSetPercent(0,i);
			printf("color change\n");        
			sleep(1);
			pwmSetPercent(50,i);
			printf("color change\n");
			sleep(1);
			pwmSetPercent(100,i);
			printf("color change\n");
			sleep(1);   
	//	}
	}
}

int FNDClock(int input)
{
	printf("fnd init!\n");
	int work = input;
	if(work == 0)
	{
		FND_write(0,0);
		tim = 0;
		timCount = 0;
	}
	unsigned int sec = 10;
	unsigned int min = 1000;

	printf("fnd write init!\n");
	//while(work)
	//{
		printf("FNDwhile!\n");
	  if(timCount <59)
		{
			tim = tim + sec;
			timCount++;
			printf("timCount < 59!\n");
		}
	  else
		{
			tim -= 590;
			tim += min;
			timCount = 0;
			printf("timCount > 60!\n");
		//}
		printf("tim is: %d\n", tim);
		if(FND_write(tim, 1000) != 0)printf("FND write failed!\n");
		sleep(1);
	}
} 
