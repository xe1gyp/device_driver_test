#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)cpp

all:
	mkdir build
	cp configure.default configure
	chmod 777 configure
	cd build; ../configure --host=$(HOST) CC=$(CC) CPP=$(CPP)
	make -C build

install:
	cp build/misc/mke2fs ../../../bin

clean:
	rm -rfv build
	rm -fv configure
