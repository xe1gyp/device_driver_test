#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS=alsa_app

all: $(SUBDIRS)

install:
	make -C alsa install

alsa_app:
	make -C alsa

clean:
	make -C alsa clean
