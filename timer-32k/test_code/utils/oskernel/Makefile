#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc

TARGETS = priority_parser
obj-m := priority_test.o

all: $(TARGETS)
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/oskernel KERN=26 modules

install:
	cp priority_test.ko ../../mods
	cp priority_parser ../../bin

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .sdio* .tmp_versions
	rm -vf priority_parser
