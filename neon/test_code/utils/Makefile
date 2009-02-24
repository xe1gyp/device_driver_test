#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS=test_app

all: $(SUBDIRS)

install:
	make -C test install

test_app:
	make -C test

clean:
	make -C test clean
