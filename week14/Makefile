CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: tempTest accelMagGyrotest

tempTest : tempTest.c libMyPeri.a temp.h
	$(CC) tempTest.c -l MyPeri -L. -o tempTest
	scp ./tempTest ecube@192.168.1.83:/home/ecube/week14

accelMagGyrotest : accelMagGyrotest.c libMyPeri.a accelMagGyro.h
	$(CC) accelMagGyrotest.c -l MyPeri -L. -o accelMagGyrotest
	scp ./accelMagGyrotest ecube@192.168.1.83:/home/ecube/week14

libMyPeri.a : temp.o accelMagGyro.o
	$(AR) rc libMyPeri.a temp.o accelMagGyro.o

temp.o : temp.h temp.c
	$(CC) -c temp.c -o temp.o

accelMagGyro.o : accelMagGyro.h accelMagGyro.c
	$(CC) -c accelMagGyro.c -o accelMagGyro.o

clean :
	rm *.o
	rm *.a
