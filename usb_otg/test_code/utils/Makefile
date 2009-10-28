#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS = testusb_app evtest_app microcom_app

all: $(SUBDIRS)

install:
	make -C testusb install
	make -C evtest install
	make -C microcom install

testusb_app:
	make -C testusb

evtest_app:
	make -C evtest

microcom_app:
	make -C microcom

clean:
	make -C testusb clean
	make -C evtest clean
	make -C microcom clean
