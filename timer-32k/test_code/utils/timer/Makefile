#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
#===============================================================================

CC := $(CROSS_COMPILE)gcc
ifneq ($(strip $(TESTSUITES_ROOT)),)
 SOURCE_DIR := ${TESTSUITES_ROOT}/timer-32k/test_code/utils/timer
else
 SOURCE_DIR := ${PWD}/utils/timer
endif


MODULES = gptimer_request.o gptimer_request_specific.o timer32value.o

obj-m:=$(MODULES)

all: $(TARGETS)
	make -C $(KDIR) SUBDIRS=$(SOURCE_DIR) KERN=26 modules

install:
	cp -v $(MODULES:.o=.ko) ../../mods

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .tmp_versions .gptimer* modules.order
	rm -vf $(TARGETS)
