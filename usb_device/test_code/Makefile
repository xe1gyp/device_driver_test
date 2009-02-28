###########################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for USB Device tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile USBHost test programs.
#
# Author: Axel Castaneda Gonzalez (x0055901@ti.com)
#
# Created on: 7 Jun 2007
#
# Change Log:
############################################################################

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
