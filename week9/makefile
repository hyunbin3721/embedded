all:final.elf

final.elf:main.c 1.o 2.o
	gcc -o final.elf main.c 1.o 2.o

1.o: 1.c myProject.h
	gcc -c 1.c

2.o: 2.c myProject.h
	gcc -c 2.c
clean:
	rm 1.o 2.o final.elf
