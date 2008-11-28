#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc

TARGETS = stat_analyzer
MODULES = dma_stat.o dma_buf_logic.o dmaunlnk.o dmastatic_chain.o dmadynamic_chain.o dmatest.o dmaoddbuffer.o
obj-m:=$(MODULES)

all: $(TARGETS)
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/dma KERN=26 modules

install:
	cp -v $(MODULES:.o=.ko) ../../mods
	cp -v stat_analyzer ../../bin

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .tmp_versions .dma*
	rm -vf $(TARGETS)
