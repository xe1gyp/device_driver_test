###########################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for Camera tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile Timer test programs.
#
# Author: Misael Lopez Cruz (x0052729@ti.com)
#
# Created on: 09 May 2007
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
	make -C utils install
	mkdir -p scripts/test
	mkdir -p scripts/tmp

clean:
	rm -rvf $(DESTDIR)
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
