#===============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
#===============================================================================

OBJS := utilities install

all: $(OBJS)

utilities:
	make -C utils

install:
	mkdir -p mods
	make -C utils install
	mkdir -p scripts/test
	mkdir -p scripts/tmp

clean:
	rm -rvf mods
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
