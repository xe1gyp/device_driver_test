###########################################################################
# (C) Copyright Texas Instruments, 2004. All Rights Reserved.
#
# Name of the file: Makefile
#
# Author: Rodrigo Castaneda (rodrigo@ti.com)
#
# Created on: 28/11/2006
#
# Change Log:
############################################################################


# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
CFLAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

#scull-objs := main.o pipe.o access.o

obj-m	:= dvr_test1.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)
KRNL16_DIR = /vobs/wtbu/OMAPSW_L/linux/kernel_org/2.6_kernel/

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

#depend .depend dep:
#	$(CC) $(CFLAGS) -M *.c > .depend


#ifeq (.depend,$(wildcard .depend))
#include .depend
#endif
