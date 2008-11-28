# ==============================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ==============================================================================

#SUBDIRS timer_app oskernel_app
SUBDIRS = timer_app

all: $(SUBDIRS)

install:
	make -C timer install
	#make -C oskernel install

timer_app:
	make -C timer

#oskernel_app:
	#make -C oskernel

clean:
	make -C timer clean
	#make -C oskernel clean
