#ifndef _temperature_h_
#define _temp_h_
#define filename "/dev/spidev1.0"

int temp_init(void);
double temp_read(void);
int temp_exit(void);

#endif //_temperature_h_
