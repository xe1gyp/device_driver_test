#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = gpadc_modules_dir

all: $(SUBDIRS)

install:
	make -C gpadc_modules install

gpadc_modules_dir:
	make -C gpadc_modules

clean:
	make -C gpadc_modules clean
