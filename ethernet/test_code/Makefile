###############################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file: ethernet/
#
# Brief description of the contents of the file:
# Top Makefile for the ethernet test suite 
#
# Detailed description of the contents of the file:
# This Makefile is used to compile ethernet test suite
#
# to compile execute the following command:
#
# make \
# CROSS_COMPILE=<toolchain.prefix> \
# KDIR=<kernel.path> \
# HOST=<host.prefix> 
#
#
# Author: Abraham Arce
#
# Created on: Mon Mar 10 23:02:20 CDT 2008
#
# Change Log:
#			Abraham Arce - March 10th 2008 - Update
#			- Update Makefile, est.cf, test_runner.sh
#			and scenario files
#
###############################################################################
OBJS := install

all:$(OBJS)

install:
	mkdir -p scripts/test
	mkdir -p scripts/tmp
clean:
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
