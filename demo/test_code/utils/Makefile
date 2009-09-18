#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = applications_directory modules_directory

all: $(SUBDIRS)

applications_directory:
	make -C applications

modules_directory:
	make -C modules

install:
	make -C applications install
	make -C modules install

clean:
	make -C applications clean
	make -C modules clean
