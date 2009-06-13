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
# Change Log:
# 		Abraham Arce Moreno - Update - Sat Mar  8 20:14:22 CST 2008
#		- Changes to Header File
#		- Changes to scenario files
#		Abraham Arce - Update - Wed Mar 12 00:19:18 CDT 2008
#		- Add new test cases
#
#		
############################################################################


CC := $(CROSS_COMPILE)gcc

BINARIES := utils/lmsensor/prog/dump/i2cdump \
	utils/lmsensor/prog/dump/i2cset \
	utils/lmsensor/prog/detect/i2cdetect \
	utils/lmsensor/prog/dump/i2c_master_test \
	utils/lmsensor/prog/dump/i2c_slave_test
LIBDIR := utils/lmsensor/lib/
LIBRARIES := libsensors.so.3.0.8 libsensors.a
LIBRARIES_NM := libsensors.so.3.0.8
LIBRARIES_LNK := libsensors.so libsensors.so.3

DESTDIR := ${PWD}/bin
OBJS := utilities install

all: $(OBJS)

utilities:
	make -C utils/lmsensor user CC=$(CC) LINUX=$(KDIR)

install:
	mkdir -p $(DESTDIR)
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	cp -v $(BINARIES) bin/
	for i in $(LIBRARIES); do cp -v $(LIBDIR)/$$i utils/lib/; done
	cd utils/lib; for i in $(LIBRARIES_LNK); do if [ ! -s $$i ]; then ln -vs $(LIBRARIES_NM) $$i ; fi; done

clean:
	make -C utils/lmsensor clean
	rm -vf bin/* lib/*
	rm -rvf $(DESTDIR)
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	for i in $(LIBRARIES); do rm -vf utils/lib/$$i; done
