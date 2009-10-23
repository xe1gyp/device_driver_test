###########################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:  
#
# Brief description of the contents of the file:
#       Makefile for hdq tests 
#
# Detailed description of the contents of the file:
# This Makefile is used to compile hdq test programs.  
#
# Author: 
#
# Created on: 
#
# Change Log:
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
