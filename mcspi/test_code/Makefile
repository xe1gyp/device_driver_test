###########################################################################
# (C) Copyright Texas Instruments. All Rights Reserved.
#
# Name of the file: Makefile
#
# Brief description of the contents of the file:
# Makefile for McSPI test suite
#
# Detailed description of the contents of the file:
# This Makefile is used to compile mcspi test suite
#
# Author: Abraham Arce
#
# Created on: Sat Mar  8 16:07:14 CST 2008
#
############################################################################
OBJS := modules install

all:$(OBJS)

modules:
	make -C utils

install:
	mkdir -p modules/
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	make -C utils install
clean:
	make -C utils clean
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf modules
