#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = hsuart_apps

all: $(SUBDIRS)

hsuart_apps:
	make -C hsuart_applications
	make -C hsuart_modules

install:
	make -C hsuart_applications install
	make -C hsuart_modules install

clean:
	make -C hsuart_applications clean
	make -C hsuart_modules clean
