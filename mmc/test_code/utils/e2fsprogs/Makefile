#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc

all:
	mkdir -p build
	cd build; ../configure --host=$(HOST) CC=$(CC)
	make -C build

install:
	cp build/misc/badblocks ../../bin
	cp build/misc/dumpe2fs ../../bin
	cp build/misc/e2image ../../bin
	cp build/misc/filefrag ../../bin
	cp build/misc/mke2fs ../../bin
	cp build/misc/tune2fs ../../bin
	cp build/e2fsck/e2fsck ../../bin
	cp build/e2fsck/gen_crc32table ../../bin

clean:
	#make -C build distclean
	rm -rf build
