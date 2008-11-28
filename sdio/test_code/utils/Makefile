#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

SUBDIRS=sdio_app 

all: $(SUBDIRS)

sdio_app:
	make -C sdio

clean:
	make -C sdio clean

install:
	make -C sdio install
