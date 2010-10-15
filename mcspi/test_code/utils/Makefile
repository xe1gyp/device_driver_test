#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = mcspi_modules_dir

all: $(SUBDIRS)

install:
	make -C mcspi_modules install
	make -C mcspi_spidev install

mcspi_modules_dir:
	make -C mcspi_modules

mcspi_spidev_dir:
	 make -C mcspi_spidev

clean:
	make -C mcspi_modules clean
