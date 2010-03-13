#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
#===============================================================================

ifeq ($(SUITE_DIR),)
SUITE_DIR=$(PWD)
endif

DESTDIR := $(SUITE_DIR)/mods
OBJS := utilities install

all: $(OBJS)

utilities:
	make -C utils

install:
	mkdir -p ${DESTDIR}
	make -C utils install
	mkdir -p scripts/test
	mkdir -p scripts/tmp

clean:
	rm -rfv ${DESTDIR}
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
