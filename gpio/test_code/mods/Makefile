###########################################################################
# (C) Copyright Texas Instruments, 2008. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
# Makefile for GPIO test code
#
# Detailed description of the contents of the file:
# This Makefile is used to compile gpio test code
#
# Author: Omar Ramirez
#
# Created on: Sat Mar  8 16:07:14 CST 2008
#
# Change Log:
#
############################################################################

CC:=$(CROSS_COMPILE)gcc
KERN:=26

ifneq ($(KERNELRELEASE),)

	ifeq ($(KERN),26)
		obj-m := gpio_test.o
	else
		export-objs :=  gpio_test.o
		obj-m += gpio_test.o
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

clean:
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions
	$(RM) -r .*.flags .26 Module.symvers modules.order
