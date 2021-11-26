obj-m := hw1.o
KDIR := /home/lee_woong_hee/embedded_project/linux_kernel/
PWD := $(shell pwd)
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	-rm *.o, *.mod.c,*.cmd modules.order Module.symvers
