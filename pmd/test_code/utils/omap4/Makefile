# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================

CC := $(CROSS_COMPILE)gcc

ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif

TARGETS=system_time
obj-m := clock_test.o pd_test.o cd_test.o


all:
	make -C $(KDIR) SUBDIRS=$(SUITE_DIR)/utils/omap4 KERN=26 modules
	$(CC) $(CFLAGS) system_time.c -o system_time

install:
	cp -vf clock_test.ko ../../modules
	cp -vf pd_test.ko ../../modules
	cp -vf cd_test.ko ../../modules
	cp -vf $(TARGETS) ../../bin

clean:
	rm -vrf *.ko .*.cmd *.o *.mod.* *Module* .tmp_versions .clock_test.o.d
	rm -vf $(TARGETS)
