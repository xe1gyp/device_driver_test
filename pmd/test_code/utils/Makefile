# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================

#SUBDIRS=dhry_app omap3_app
#SUBDIRS=omap3_app

SUBDIRS=omap4_app

all: $(SUBDIRS)

dhry_app:
	make -C dhry

omap3_app:
	make -C omap3

omap4_app:
	make -C omap4

clean:
	make -C dhry clean
	make -C omap4 clean

install:
	make -C dhry clean
	make -C omap4 install
