# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================

CC=$(CROSS_COMPILE)gcc

ifneq ($(strip $(TESTSUITES_ROOT)),)
 SOURCE_DIR := ${TESTSUITES_ROOT}/power_management/test_code/utils/omap3/
else
 SOURCE_DIR := ${PWD}/utils/omap3
endif

TARGETS= time_difference
MODULES= prcm_test.o clockdomain_test.o powerdomain_test.o
obj-m:=$(MODULES)
ccflags-y += -Iarch/arm

all:
	make -C $(KDIR) SUBDIRS=$(SOURCE_DIR)  KERN=26 modules
	$(CC) time_difference.c -o time_difference

install:
	cp -v $(MODULES:.o=.ko) ../../mods
	cp -vf $(TARGETS) ../../bin

clean:
	rm -vrf *.ko .*.cmd *.o *.mod.* *Module* .tmp_versions *.o.d
	rm -vf $(TARGETS) modules.order
