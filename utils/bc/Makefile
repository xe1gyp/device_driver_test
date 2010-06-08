# ============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#            (C) Copyright Texas Instruments, Incorporated
#                          All Rights Reserved
#
# Use of this software is controlled by the terms and conditions found
# in the license agreement under which this software has been supplied.
# ============================================================================

CC := $(CROSS_COMPILE)gcc

all:
	mkdir -p build
	cd build; ../configure --host=$(HOST) CC=$(CC)
	make -C build

install:
	cp build/bc/bc ../bin/

clean:
	rm -rf build

