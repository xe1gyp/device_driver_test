#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS = e2fsprogs_dir mkdosfs_dir

all: $(SUBDIRS)

install:
	make -C e2fsprogs install
	make -C mkdosfs install

e2fsprogs_dir:
	make -C e2fsprogs

mkdosfs_dir:
	make -C mkdosfs

clean:
	make -C e2fsprogs clean
	make -C mkdosfs clean
