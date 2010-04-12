# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================

CC=$(CROSS_COMPILE)gcc

TARGETS=time_difference
obj-m := clock_test.o prcm_test.o resource_test.o co_test.o

all:
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/omap3 KERN=26 modules
	$(CC) time_difference.c -o time_difference

install:
	cp -vf clock_test.ko ../../mods
	cp -vf prcm_test.ko ../../mods
	cp -vf resource_test.ko ../../mods
	cp -vf co_test.ko ../../mods
	cp -vf $(TARGETS) ../../bin

clean:
	chmod 775 .clock_test.o.d
	rm -vrf *.ko .*.cmd *.o *.mod.* *Module* .tmp_versions .clock_test.o.d
	rm -vf $(TARGETS)
