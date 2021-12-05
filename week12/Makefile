CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: ledtest buttontest buzzertest

buzzertest : buzzertest.c libMyPeri.a buzzer.h
	$(CC) buzzertest.c -l MyPeri -L. -o buzzertest

buttontest : buttontest.c libMyPeri.a button.h
	$(CC) buttontest.c -l MyPeri -L. -o buttontest -lpthread

ledtest : ledtest.c libMyPeri.a led.h
	$(CC) ledtest.c -l MyPeri -L. -o ledtest

libMyPeri.a : led.o button.o buzzer.o
	$(AR) rc libMyPeri.a led.o button.o buzzer.o

led.o : led.h led.c
	$(CC) -c led.c -o led.o

button.o : button.h button.c
	$(CC) -c button.c -o button.o

buzzer.o : buzzer.h buzzer.c
	$(CC) -c buzzer.c -o buzzer.o

clean :
	rm *.o
	rm *.a