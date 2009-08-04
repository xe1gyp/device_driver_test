#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
#===============================================================================

CC := $(CROSS_COMPILE)gcc

MODULES = gptimer_request.o gptimer_request_specific.o

obj-m:=$(MODULES)

all: $(TARGETS)
	make -C $(KDIR) -I$(KDIR)/include SUBDIRS=$(PWD)/utils/timer KERN=26 modules

install:
	cp -v $(MODULES:.o=.ko) ../../mods

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .tmp_versions .gptimer* modules.order
	rm -vf $(TARGETS)
