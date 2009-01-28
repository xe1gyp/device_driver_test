#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS= rng_app

all: $(SUBDIRS)

install:
	make -C rng install

rng_app:
	make -C rng

clean:
	make -C rng clean
