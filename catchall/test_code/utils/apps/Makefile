#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
#===============================================================================

CC := $(CROSS_COMPILE)gcc

ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif

MODULES = debug_module_test.o

ifeq ($(TARGET_PLATFORM), OMAP_4430)
MODULES += debug_module_test.o
endif

obj-m:=$(MODULES)

all: $(TARGETS)
	make -C $(KDIR) SUBDIRS=$(SUITE_DIR)/utils/apps KERN=26 modules

install:
	cp -v $(MODULES:.o=.ko) $(SUITE_DIR)/modules

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.ko.cmd *.o *.o.cmd .debug* *.mod.c .tmp_versions modules.order *Module*
	rm -vf $(TARGETS)

