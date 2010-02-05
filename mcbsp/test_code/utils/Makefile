#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = mcbsp_modules_dir

all: $(SUBDIRS)

install:
	make -C mcbsp_modules install

mcbsp_modules_dir:
	make -C mcbsp_modules

clean:
	make -C mcbsp_modules clean
