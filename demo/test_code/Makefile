###########################################################################
# (C) Copyright Texas Instruments. All Rights Reserved.
#
# Name of the file: Makefile
#
# This Makefile is used to compile demo test suite  
#
# Author: Abraham Arce
#
# Created on: Thu Sep 17 11:11:00 CDT 2009
#
############################################################################
OBJS :=  utilities install

all:$(OBJS)

utilities:
	make -C utils

install:
	mkdir -p bin
	mkdir -p modules
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	make -C utils install
clean:
	make -C utils clean
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf modules
	rm -rf bin

