#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS=keypad_app

all: $(SUBDIRS)

install:
	make -C keypad install

keypad_app:
	make -C keypad

clean:
	make -C keypad clean
