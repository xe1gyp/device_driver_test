###############################################################################
# (C) Copyright Texas Instruments - All Rights Reserved
#
# Name of the file: Makefile
#
# Brief description of the contents of the file:
# Top Makefile for Neon Test Suite
#
# To compile execute the following command:
#
# make \
# CROSS_COMPILE=<toolchain.prefix> \
# KDIR=<kernel.path> \
# HOST=<host.prefix>
#
# Author: TBD
# Created on: Tue Feb 24 04:08:48 CST 2009
#
###############################################################################
OBJS := utilities install

all:$(OBJS)

utilities:
	make -C utils
install:
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	mkdir -p bin/
	make -C utils install
clean:
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf bin/
