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
OBJS := modules install

all:$(OBJS)

modules:
	make -C mods 
install:
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	mkdir -p bin/
clean:
	make -C mods clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf bin/
