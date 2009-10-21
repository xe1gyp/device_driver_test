# 
# Makefile for compilation of mcspi test code
# Testing the McSPI
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
# Revision History
# ================
# 21-Oct-2009 Manjunath GK     Cleaned up - removed 2.4 kernel references
#
#

    obj-m		+=	omap2_mcspi_datatest.o
#    obj-m		+=	omap2_mcspi_irqtest.o
#    obj-m		+=	omap2_mcspi_wktest.o
#    obj-m		+=	omap2_spi_seclcd.o

    default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26
