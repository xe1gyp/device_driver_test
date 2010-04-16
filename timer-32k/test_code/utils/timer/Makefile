#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
#===============================================================================

ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif
CC := $(CROSS_COMPILE)gcc

MODULES = gptimer_request.o gptimer_request_specific.o timer32value-smp.o timer32value.o

obj-m:=$(MODULES)

all: $(TARGETS)
	make -C $(KDIR) -I$(KDIR)/include SUBDIRS=$(SUITE_DIR)/utils/timer KERN=26 modules

install:
	cp -v $(MODULES:.o=.ko) ../../modules

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .tmp_versions .gptimer* modules.order .*.ko .*.o.* .*.ko.*
	rm -vf $(TARGETS)
