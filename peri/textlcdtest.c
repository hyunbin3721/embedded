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
#include "textlcd.h"

#define TEXTLCD_DRIVER_NAME		"/dev/peritextlcd"

int main(void)
{
    textlcdInit(); //text LCD use
    lcdtextwrite("  hello world!  ", "   first line   ", 1); // 1번째 줄에 쓰기 (16칸 엄수)
    sleep(5);
    lcdtextwrite("  hello world!  ", "  second  line  ", 2); // 2번쨰 줄에 쓰기 (16칸 엄수)
    sleep(5);
    textlcdclear(); // 줄 초기화
    sleep(1); 
    lcdtextwrite("textlcd test end", NULL, 1);
    sleep(1);
    textlcdExit();
    return 0;
}