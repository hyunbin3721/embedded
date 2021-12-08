CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: textlcdtest colorledtest FNDtest

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
