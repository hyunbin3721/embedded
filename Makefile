CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: ledtest buttontest buzzertest textlcdtest colorledtest FNDtest

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

textlcdtest : textlcd.c libMyPeri.a textlcd.h textlcddrv.h
	$(CC) textlcdtest.c -l MyPeri -L. -o textlcdtest

colorledtest : colorledtest.c libMyPeri.a colorled.h
	$(CC) colorledtest.c -l MyPeri -L. -o colorledtest -lpthread

FNDtest : FNDtest.c libMyPeri.a FND.h
	$(CC) FNDtest.c -l MyPeri -L. -o FNDtest

libMyPeri.a : FND.o textlcd.o colorled.o
	$(AR) rc libMyPeri.a FND.o textlcd.o colorled.o

FND.o : FND.h FND.c
	$(CC) -c FND.c -o FND.o

textled.o : textlcd.h textlcddrv.h textlcd.c
	$(CC) -c textled.c -o textled.o

colorled.o : colorled.h colorled.c
	$(CC) -c colorled.c -o colorled.o

clean :
	rm *.o
	rm *.a