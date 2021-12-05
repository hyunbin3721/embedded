#ifndef _BUZZER_H_
#define _BUZZER_H_

int buzzerInit(void);           // buzzer 포함 함수
int buzzerPlaySong(int scale);  // buzzer on
int buzzerStopSong(void);       // buzzer off
int buzzerExit(void);           // buzzer exit

#endif
