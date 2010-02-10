###########################################################################
# (C) Copyright Texas Instruments, 2008. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:  
#
# Brief description of the contents of the file:
# Makefile for GPIO test suite 
#
# Detailed description of the contents of the file:
# This Makefile is used to compile gpio test suite  
#
# Author: Abraham Arce
#
# Created on: Sat Mar  8 16:07:14 CST 2008
#
# Change Log:
#
############################################################################
OBJS := modules install

all:$(OBJS)

modules:
	$(MAKE) -C utils

install:
	mkdir -p modules/
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	$(MAKE) -C utils install
clean:
	$(MAKE) -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf modules/

