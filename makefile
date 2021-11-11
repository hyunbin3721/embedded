all:final.elf

final.elf:main.o head.o
	gcc -o output.elf main.o head.o

main.o: main.c myProject.h
	gcc -c main.c myProject.h

head.o: 1.c 2.c myProject.h
	gcc -c 1.c 2.c  myProject.h
clean:
	rm main.o head.o
