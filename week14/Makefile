CC = arm-linux-gnueabi-gcc
AR = arm-linux-gnueabi-ar

all: accelMagGyroTest temperatureTest

accelMagGyroTest : accelMagGyroTest.c libMyPeri.a accelMagGyro.h
	$(CC) accelMagGyroTest.c -l MyPeri -L. -o accelMagGyroTest
	#scp ./accelMagGyroTest ecube@192.168.1.83:/home/ecube/week14

temperatureTest : temperatureTest.c libMyPeri.a temperature.h
	$(CC) temperatureTest.c -l MyPeri -L. -o temperatureTest
	#scp ./temperatureTest ecube@192.168.1.83:/home/ecube/week14


libMyPeri.a : accelMagGyro.o temperature.o
	$(AR) rc libMyPeri.a accelMagGyro.o temperature.o

accelMagGyro.o : accelMagGyro.h accelMagGyro.c
	$(CC) -c accelMagGyro.c -o accelMagGyro.o

temperature.o : temperature.h temperature.c
	$(CC) -c temperature.c -o temperature.o

clean :
	rm *.o
	rm *.a
