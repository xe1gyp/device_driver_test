# 
# /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/mcspi/test_code/
#
# Makefile for compilation of 2.4 or 2.6 kernel based modules for 
# Testing the McSPI
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
#
# Author : Syed Mohammed Khasim
#

#Trying to make this work for 2.4 and 2.6 kernel
#KDIR        := /vobs/wtbu/OMAPSW_L/linux/kernel_bk/2.6_kernel
#KDIR	:= /vobs/wtbu/OMAPSW_L/linux/kernel_mv/2.4_kernel-arm11
#KDIR	:= /vobs/wtbu/OMAPSW_L/linux/kernel_mv/2.6_kernel-arm11
KDIR        := /vobs/wtbu/OMAPSW_L/linux/kernel_org/2.6_kernel

ifneq ($(KERNELRELEASE),)
ifeq ($(KERN),26)
    obj-m		+=	omap2_mcspi_datatest.o
    obj-m		+=	omap2_mcspi_irqtest.o
    obj-m		+=	omap2_mcspi_wktest.o
    obj-m		+=	omap2_spi_seclcd.o
else
    export-objs		:=	omap2_mcspi_datatest.o
    obj-m +=omap2_mcspi_datatest.o
    export-objs		:=	omap2_mcspi_irqtest.o
    obj-m +=omap2_mcspi_irqtest.o
    export-objs		:=	omap2_mcspi_wktest.o
    obj-m +=omap2_mcspi_wktest.o
    export-objs		:=	omap2_spi_seclcd.o
    obj-m +=omap2_spi_seclcd.o
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
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26
