# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================

CC=$(CROSS_COMPILE)gcc

TARGETS= system_time vfp_asm
obj-m := clock_test.o resource_test.o clockdomain_test.o powerdomain_test.o constraints.o

all:
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/omap3 KERN=26 modules
	$(CC) system_time.c -o system_time
	$(CC) cpuIdle.c -o cpuIdle
	$(CC) -mcpu=cortex-a8 -march=armv7-a -mfpu=vfpv3 -mfloat-abi=softfp vfp_asm.c -W -o vfp_asm

install:
	cp -vf clock_test.ko ../../mods
	cp -vf resource_test.ko ../../mods
	cp -vf clockdomain_test.ko ../../mods
	cp -vf powerdomain_test.ko ../../mods
	cp -vf $(TARGETS) ../../bin

clean:
	rm -vrf *.ko .*.cmd *.o *.mod.* *Module* .tmp_versions *.o.d
	rm -vf $(TARGETS)
