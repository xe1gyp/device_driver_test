###########################################################################
# (C) Copyright Texas Instruments, 2004. All Rights Reserved.
#
# Name of the file: Makefile
# Author: Abraham Arce(x0066660@ti.com)
# Created on: 4/1/2007
############################################################################

CC:=$(CROSS_COMPILE)gcc
KERN:=26

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
CFLAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)

obj-m	:= twl4030led_test.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions
