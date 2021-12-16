#ifndef _FND_h_
#define _FND_h_

#define MAX_FND_NUM 6
#define FND_DATA_BUFF_LEN (MAX_FND_NUM + 2)

typedef struct FNDdata
{	
	char DataNumber[FND_DATA_BUFF_LEN]; //숫자 / 0-9
	char DataDot[FND_DATA_BUFF_LEN]; //소수점 / 0 or 1
	char DataPower[FND_DATA_BUFF_LEN]; //전원공급 / 0 or 1
	
}inputstruct;

int FND_init(void);
int FND_write(unsigned int numInput, unsigned int dotInput);
int FND_on(void);
int FND_off(void);
int FND_exit(void);
int FND_on_clock(void);

#endif //_FND_h_
