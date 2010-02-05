#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS=lmbench3_app llcbench_apps

all: $(SUBDIRS)

lmbench3_app:
	make -C lmbench3

llcbench_apps:
	make -C llcbench cache-bench

clean:
	make -C lmbench3 clean
	make -C llcbench clean

