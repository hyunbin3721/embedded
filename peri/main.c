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
#include <sys/types.h>
#include <sys/shm.h>

/*
#include <sys/mman.h>
#include <mad.h>
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
static char *file;

static char *tmp;
static char *fileBuf[100];
static char nameBuf[100][50];
static char line1Buf[17];
static char line2Buf[17];
static int listIndex = 0;
static int listIndexMax = 0;

static int LEDstatus = 0;
static int homeStatus = 0;
static int menuStatus = 0;

BUTTON_MSG_T recieveButton;


int ledMid(void);//led중 절반만 ON
int textlcdBufferWrite(void);//textlcd작성함수

int VOLUP(void);//Button별 함수
int VOLDOWN(void);
int HOME(void);
int SEARCH(void);
int BACK(void);
int MENU(void);

int main(void)
{
//mp3파일 디렉토리 내용 읽기==============================================
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
			strcpy(nameBuf[listIndex],dir->d_name);
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
	
	closedir(list);
//======================================================================
	
	ledLibInit();ledMid();//LED시작, LED1,2,3,4 ON
	pwmLedInit();//color led 시작 

	textlcdInit();lcdtextwrite(fileBuf[listIndex]," ", 1);textlcdBufferWrite();//textled 시작. 첫번째 제목 표시
	FND_init();FND_on_clock();//FND 시작.
	
	if(buttonInit() != 1)printf("buttonInit Failed!\n");//Button 시작 
	
	decoderPid = clockPid = -1; //fork
	
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
		int shmID = shmget((key_t)MEMADD, 1, IPC_CREAT|0666);//공유 메모리 생성
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
		
		/*
		if((mp3 = open()) < 0)
		{
			printf("mp3 Open failed!\n");
		}
		*/
		
		while(1)
		{
			if(*((int*)shmemAddr))//디코딩,재생
			{
				printf("decode\n");
				
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
		
		int shmID = shmget((key_t)MEMADD, 1, IPC_CREAT|0666);//공유 메모리 생성
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
	printf("led status: %d\n", LEDstatus);
	ledOnOff(LEDstatus,1);
	LEDstatus++;
	if(LEDstatus > 7)LEDstatus = 7;
	printf("led after status: %d\n", LEDstatus);
}

int VOLDOWN(void)
{
	printf("led status: %d\n", LEDstatus);
	ledOnOff(LEDstatus,0);
	LEDstatus--;
	if(LEDstatus < 0)LEDstatus = 0;
	printf("led after status: %d\n", LEDstatus);
}

int HOME(void)
{
	
	if(listIndex == listIndexMax)
	{
		listIndex = 0;
		textlcdBufferWrite();
		homeStatus = 0;
	}
	else
	{
		listIndex = listIndexMax;
		textlcdBufferWrite();
	}
	
}

int MENU(void)
{
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
	textlcdBufferWrite();			
}

int SEARCH(void)
{
	listIndex++;
	if(listIndex > listIndexMax)listIndex = listIndexMax;
	textlcdBufferWrite();
}

 int textlcdBufferWrite(void)
 {
	 memcpy(line1Buf, &nameBuf[listIndex][0],16);
	 memcpy(line2Buf, &nameBuf[listIndex][16],16);
     line1Buf[16] = '\0'; line2Buf[16] = '\0';
     for(int cnt = 1; cnt < 3; cnt++)lcdtextwrite(line1Buf,line2Buf,cnt);
 }
