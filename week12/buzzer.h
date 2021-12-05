#ifndef _BUZZER_H_
#define _BUZZER_H_

int buzzerInit(void);           // buzzer 초기화 함수
int buzzerPlaySong(int scale);  // scale 음계를 울리는 함수
int buzzerStopSong(void);       // 소리 끄는 함수
int buzzerExit(void);           // buzzer exit

#endif