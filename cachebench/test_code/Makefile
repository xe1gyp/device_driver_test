###########################################################################
# (C) Copyright Texas Instruments, 2009. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for CACHEBENCH
#
# Detailed description of the contents of the file:
# This Makefile is used to compile LMBENCH test programs.
#
# Author: Ricardo Perez Olivares (x0081762@ti.com)
#
# Created on: 02 Oct 2009
#
# Change Log:
############################################################################

DESTDIR := bin
OBJS	:= utilities install
CB_CC=$(CROSS_COMPILE)gcc


all: $(OBJS)

utilities:
	make CB_CC=$(CB_CC) cache-bench -C utils/

install:
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	mkdir -p $(DESTDIR)
	cp -rf utils/cachebench/cachebench $(DESTDIR)

clean:
	make -C utils/ clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rfv $(DESTDIR)

