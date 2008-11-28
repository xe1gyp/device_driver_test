#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
EXTRA_CFLAGS += -I$(KDIR)

TARGETS = timer_parser
obj-m := test32ktimer.o

all: $(TARGETS)
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/timer KERN=26 modules

install:
	cp test32ktimer.ko ../../mods
	cp timer_parser ../../bin

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .sdio* .tmp_versions
	rm -vf timer_parser
