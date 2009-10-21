# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================

CC=$(CROSS_COMPILE)gcc

TARGETS=time_difference system_time
obj-m := clock_test.o prcm_test.o resource_test.o co_test.o cd_test.o pd_test.o

all:
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/omap3 KERN=26 modules
	$(CC) time_difference.c -o time_difference
	$(CC) system_time.c -o system_time

install:
	cp -vf clock_test.ko ../../mods
	cp -vf prcm_test.ko ../../mods
	cp -vf resource_test.ko ../../mods
	cp -vf co_test.ko ../../mods
	cp -vf cd_test.ko ../../mods
	cp -vf pd_test.ko ../../mods
	cp -vf $(TARGETS) ../../bin

clean:
	rm -vrf *.ko .*.cmd *.o *.mod.* *Module* .tmp_versions
	rm -vf $(TARGETS)
