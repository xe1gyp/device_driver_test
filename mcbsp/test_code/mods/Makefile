# Makefile for compilation of 2.4 or 2.6 kernel based modules for 
# Testing the McBSP
# NOTE: 'make KERN=26' to compile for 2.6 kernel
#
# NOTE: with mv kernel, we may need to use insmod with -f option
#
# Copyright (C) 2004-2005 Texas Instruments, Inc.
#
# This package is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.

CC:=$(CROSS_COMPILE)gcc
KERN:=26

ifneq ($(KERNELRELEASE),)
ifeq ($(KERN),26)
    obj-m       := omap_mcbsp_test.o
else
    export-objs     :=  omap_mcbsp_test.o
    obj-m +=omap_mcbsp_test.o
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
	chmod 775 .omap_mcbsp_test.o.d
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26 .omap_mcbsp_test.o.d
