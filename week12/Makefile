ledtest : ledtest.c libMyPeri.a led.h
	arm-linux-gnueabi-gcc ledtest.c -l MyPeri -L. -o ledtest -lpthread

libMyPeri.a : led.o
	arm-linux-gnueabi-ar rc libMyPeri.a led.o

led.o : led.h led.c
	arm-linux-gnueabi-gcc -c led.c -o led.o

clear :
	rm *.o
	rm libMyPeri.a