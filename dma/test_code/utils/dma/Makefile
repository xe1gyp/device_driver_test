#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
#===============================================================================

CC := $(CROSS_COMPILE)gcc

MODULES = dma_single_channel.o \
  dma_chain_transfer.o \
  dma_priority_high.o \
  dma_priority_low.o \
  dma_stop_transfer.o \
  dma_transfer_query.o \
  dma_addrmode_constant.o \
  dma_addrmode_postinc.o \
  dma_addrmode_singleidx.o \
  dma_addrmode_doubleidx.o \
  dma_burst_disabled.o \
  dma_burst_mode4.o \
  dma_burst_mode8.o \
  dma_burst_mode16.o \
  dma_endianism_big.o \
  dma_chain_static.o \
  dma_chain_dynamic.o \
  dma_thread_access.o \


obj-m:=$(MODULES)

all: $(TARGETS)
	make -C $(KDIR) SUBDIRS=$(PWD)/utils/dma KERN=26 modules

install:
	cp -v $(MODULES:.o=.ko) ../../mods

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vrf *.ko *.o *.mod.* *Module* .tmp_versions .dma* modules.order
	rm -vf $(TARGETS)
