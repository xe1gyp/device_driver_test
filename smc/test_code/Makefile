###########################################################################
# (C) Copyright Texas Instruments, 2010. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for SMC [Security Middleware Component] test
#
# Detailed description of the contents of the file:
# This Makefile is used to compile SMC test programs.
#
# Author: Praneeth Kumar Bajjuri (praneeth@ti.com)
#
# Created on: 04 March 2010
#
# Change Log:
############################################################################

DESTDIR := ${PWD}/bin
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
