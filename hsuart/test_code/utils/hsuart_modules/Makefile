# Comment/uncomment the following line to disable/enable debugging
# DEBUG = y

export CC := $(CROSS_COMPILE)gcc
export KERN := 26

ifneq ($(KERNELRELEASE),)
ifeq ($(KERN),26)
	obj-m := uart_library.o
else
	export-objs := uart_library.o
	obj-m += uart_library.o
include $(TOPDIR)/Rules.make
endif
else
	PWD=$(shell pwd)
ifeq ($(KERN),26)
	a :=$(shell touch .26)
endif

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) KERN=$(KERN) modules
endif

install:
	cp *.ko ../../modules/

clean:
	$(RM) -r *.o *.o.* *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26
