#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

OBJS := e2fsprogs-1.36_app mkdosfs-0.4_app

all: $(OBJS)

e2fsprogs-1.36_app:
	make -C e2fsprogs-1.36

mkdosfs-0.4_app:
	make -C mkdosfs-0.4

install:
	make -C e2fsprogs-1.36 install
	make -C mkdosfs-0.4 install

clean:
	make -C e2fsprogs-1.36 clean
	make -C mkdosfs-0.4 clean

