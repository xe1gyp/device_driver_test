##############################################################################
# (C) Copyright Texas Instruments, 2008. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file: usb_otg/
#
# Brief description of the contents of the file:
#       Makefile for USB OTG tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile USBOTG test programs.
#
# Author: Axel Castaneda Gonzalez (x0055901@ti.com)
#
# Created on: 7 Jun 2007
#
# Change Log:
#	12 Jan 2008 - Diego Zavala Trujillo - Add log directory to keep all
#		 log files in there.
##############################################################################

DESTDIR := bin mods
OBJS := utilities install

all: $(OBJS)

utilities:
	make -C utils

install:
	mkdir -p $(DESTDIR)
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	mkdir -p scripts/log
	make  -C utils install

clean:
	rm -rvf $(DESTDIR)
	rm -rf scripts/log
	rm -rf scripts/tmp
	rm -rf scripts/test
	make -C utils clean
