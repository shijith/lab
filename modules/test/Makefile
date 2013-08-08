obj-m := test.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CC := gcc
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
