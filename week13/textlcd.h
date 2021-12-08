#ifndef _TEXTLCD_H_
#define _TEXTLCD_H_

int textlcdInit(void); // tect LCD use
int textlcdExit(void); // text LCD turn off
int textlcdclear(void); // text LCD lineclear
int lcdtextwrite(const char *str1, const char *str2, int lineFlag); // text LCD wirte

#endif