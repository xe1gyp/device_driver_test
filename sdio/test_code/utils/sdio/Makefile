#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc

obj-m := sdio.o

all:
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/sdio KERN=26 modules

install:
	cp sdio.ko ../../mods

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .sdio* .tmp_versions
