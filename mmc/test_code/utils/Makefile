#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

OBJS := mmc_app fs_app

all: $(OBJS)

mmc_app:
	make -C mmc
fs_app:	
	make -C fs

install:
	make -C mmc install
	make -C fs install

clean:
	make -C mmc clean
	make -C fs clean
