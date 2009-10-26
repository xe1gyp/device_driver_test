###########################################################################
# (C) Copyright Texas Instruments. All Rights Reserved.
#
# Name of the file: Makefile
#
# Brief description of the contents of the file:
# Makefile for hsuart tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile hsuart test programs.
#
# Author: Abraham Arce
#
# Created on: Sat Mar  8 16:07:14 CST 2008
#
############################################################################

OBJS := utilities install

all:$(OBJS)

utilities:
	make -C utils
install:
	mkdir bin
	mkdir mods
	make -C utils install
	mkdir -p scripts/test
	mkdir -p scripts/tmp
clean:
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf bin
	rm -rf mods
