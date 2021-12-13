CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: tempTest

tempTest : tempTest.c libMyPeri.a temp.h
	$(CC) tempTest.c -l MyPeri -L. -o tempTest
	scp ./tempTest ecube@192.168.1.83:/home/ecube/week14

libMyPeri.a : temp.o
	$(AR) rc libMyPeri.a temp.o

temp.o : temp.h temp.c
	$(CC) -c temp.c -o temp.o
clean :
	rm *.o
	rm *.a
