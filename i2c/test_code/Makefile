###########################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file: i2c/
#
# Brief description of the contents of the file:
# Makefile for I2C Test Suite
#
# Detailed description of the contents of the file:
# This Makefile is used to compile I2C Test Suite by executing the following command:
#
# make \
# CROSS_COMPILE=<toolchain.prefix> \
# KDIR=<kernel.path> \
# HOST=<host.prefix>
#
# Author: Nishant Menon
#
# Created on: Tue Jun 13 12:23:42 CDT 2006
#
#
# Change Log:
# 		Abraham Arce Moreno - Update - Sat Mar  8 20:14:22 CST 2008
#		- Changes to Header File
#		- Changes to scenario files
#		Abraham Arce - Update - Wed Mar 12 00:19:18 CDT 2008
#		 - Add new test cases
#		Ricardo Perez Olivares - Update Thur Sep 1 16:30:00 CDT 2009
#		- Deleting lmsensors references
#		- Adding i2c-tools references
#
##############################################################################

CC := $(CROSS_COMPILE)gcc

BINARIES := utils/i2c-tools-3.0.2/tools/i2cdump \
		utils/i2c-tools-3.0.2/tools/i2cset \
		utils/i2c-tools-3.0.2/tools/i2cget \
		utils/i2c-tools-3.0.2/tools/i2cdetect \

DESTDIR	:= bin
OBJS := utilities install

all: $(OBJS)

utilities:
	make -C utils/i2c-tools-3.0.2/ CC=$(CC)

install:
	mkdir -p $(DESTDIR)
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	cp -v $(BINARIES) bin/

clean:
	make -C utils/i2c-tools-3.0.2 clean
	rm -vf bin/*
	rm -rvf $(DESTDIR)
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test

