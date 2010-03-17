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
	cp build/misc/badblocks ../../binaries
	cp build/misc/dumpe2fs ../../binaries
	cp build/misc/e2image ../../binaries
	cp build/misc/filefrag ../../binaries
	cp build/misc/mke2fs ../../binaries
	cp build/misc/tune2fs ../../binaries
	cp build/e2fsck/e2fsck ../../binaries
	cp build/e2fsck/gen_crc32table ../../binaries

clean:
	#make -C build distclean
	rm -rf build
