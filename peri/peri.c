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

static int msgID;
static int forkStatus;
static pid_t decoderPid, clockPid;

static int fd  = 0;
static int mp3 = 0;
static char *tmpBuff;
static char *fileBuf[100];
static char nameBuf[100][30];
static int listIndex = 0;
static int listIndexMax = 0;
static int LEDstatus = 0;
static int VOLstatus;
static int textLcdstatus = 0;
static int homeStatus = 0;
static int menuStatus = 0;

BUTTON_MSG_T recieveButton;


int ledMid(void);
int VOLUP(void);
int VOLDOWN(void);
int HOME(void);
int SEARCH(void);
int BACK(void);
int MENU(void);
//int decode(int input);
//int FNDClock(int input);

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
	
	while((dir = readdir(list)) != NULL)
	{
		if(dir->d_type == DT_REG)
		{
			memcpy(nameBuf[listIndex],dir->d_name,15);
			nameBuf[listIndex][16]='\0';
			fileBuf[listIndex] = &nameBuf[listIndex][0];
			listIndex++;
			if(listIndex > sizeof(fileBuf))
			{
				printf("too many mp3 files!\n");
				exit(-1);
			}
			listIndexMax = listIndex;
		}
	}
	listIndex = 0; listIndexMax--;
	printf("file max index: %d\n", listIndexMax);
	
	closedir(list);
	
	/*if((fd = open(MP3_PATH "list.txt", O_RDONLY)) < 0)
	{
		printf("list open failed!\n");
	}*/
	
	ledLibInit();ledMid();//LED시작, LED1,2,3,4,5 ON
	pwmLedInit();

	textlcdInit();lcdtextwrite(fileBuf[listIndex]," ", 1);//textled 시작. 첫번째 제목 표시
	FND_init();FND_on_clock();//FND 시작.
	
	if(buttonInit() != 1)printf("buttonInit Failed!\n");
	
	decoderPid = clockPid = -1;
	
	decoderPid = fork();
	if(decoderPid < 0)
	{
		printf("decoderPid failed!\n");
		exit(-1);
	}
	
	if(decoderPid>0)clockPid = fork();


//main=========================================================			
	if(decoderPid > 0 && clockPid > 0)
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
//======================================================================		
	
//decoder PID==================================================	
	else if(decoderPid== 0 && clockPid == -1)
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
			//decode(*((int*)shmemAddr));
			//int playing = input;
	
			if(*((int*)shmemAddr))//디코딩,재생
			{
				printf("testing!\n");
				for(int i=0; i<3; i++)
				{   
					pwmSetPercent(0,i);       
					usleep(50000);
					
					pwmSetPercent(50,i);
					usleep(50000);
					
					pwmSetPercent(100,i);
					usleep(50000);   
				}
			 }
		}
	}
//======================================================================		
		
//clock PID====================================================
	else if(decoderPid > 0 && clockPid ==0)
	{
		unsigned int tim = 0;
		int timCount = 0;
		int flag = 0;
		unsigned int sec = 10;
		unsigned int min = 1000;
		
		int shmID = shmget((key_t)MEMADD, 1, IPC_CREAT|0666);
		if(shmID == -1)
		{
			printf("clock pid shmget error!\n");
			exit(-1);
		}
		char *shmemAddr;
		shmemAddr = shmat(shmID, (void*)NULL, 0);
		if(((int)(shmemAddr))== -1)
		{
			printf("clock pid shmat error!\n");
		}
		
		while(1)
		{
			if(*((int*)shmemAddr) == 0 && flag == 0)
			{
				FND_write(0,1000);
				tim = 0;
				timCount = 0;
				flag = 1;
			}
			
			while(*((int*)shmemAddr))
			{
				flag = 0;
				if(timCount <59)
				{
					tim = tim + sec;
					timCount++;
				}
				else
				{
					tim -= 590;
					tim += min;
					timCount = 0;
				}

				if(FND_write(tim, 1000) != 0)printf("FND write failed!\n");
				sleep(1);			
			}
		}
	}
//======================================================================

//fork 실패=============================================================	
	else
	{
		printf("Fork failed!\n");
		exit(-1);
	}
//======================================================================
}



int ledMid(void)
{
	for(int ledCnt = 0; ledCnt < 4; ledCnt++)
	{
		ledOnOff(ledCnt,1);
	}
	
	LEDstatus = 3;
}

//키 함수================================================================
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
	
	if(listIndex == listIndexMax)
	{
		lcdtextwrite(fileBuf[0], " ",1);
		homeStatus = 0;
	}
	else
	{
		lcdtextwrite(fileBuf[listIndexMax], " ",1);
		listIndex = listIndexMax;
	}
	//printf("to first music or last\n");
	
}

int MENU(void)
{
	//printf("song should played button\n");
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
	lcdtextwrite(fileBuf[listIndex], " ", 1);			
}

int SEARCH(void)
{
	listIndex++;
	if(listIndex > listIndexMax)listIndex = listIndexMax;
	lcdtextwrite(fileBuf[listIndexMax], " ", 1);
}
//======================================================================
/*
int decode(int input)
{
	int playing = input;
	
	if(playing)//디코딩,재생
	{
		printf("testing!\n");
		for(int i=0; i<3; i++)
		{   
			pwmSetPercent(0,i);       
			usleep(50000);
			
			pwmSetPercent(50,i);
			usleep(50000);
			
			pwmSetPercent(100,i);
			usleep(50000);   
		}
	}
}*/
/*
int FNDClock(int input)
{
	if(input == 0)
	{
		FND_write(0,0);
		tim = 0;
		timCount = 0;
	}
	unsigned int sec = 10;
	unsigned int min = 1000;

	while(input)
	{
		//printf("FNDwhile!\n");
		if(timCount <59)
		{
			tim = tim + sec;
			timCount++;
			//printf("timCount < 59!\n");
		}
		else
		{
			tim -= 590;
			tim += min;
			timCount = 0;
			//printf("timCount > 60!\n");
		}
		
		//printf("tim is: %d\n", tim);
		if(FND_write(tim, 1000) != 0)printf("FND write failed!\n");
		sleep(1);
	}
	 */
 
