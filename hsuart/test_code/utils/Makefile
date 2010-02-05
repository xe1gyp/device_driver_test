#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = hsuart_applications_dir #hsuart_modules_dir

all: $(SUBDIRS)

install:
	make -C hsuart_applications install

hsuart_applications_dir:
	make -C hsuart_applications

clean:
	make -C hsuart_applications clean
