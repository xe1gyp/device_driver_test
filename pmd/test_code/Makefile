###########################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for Power Management tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile Power Management test programs.
#
# Author:
#
# Created on:
#
# Change Log:
############################################################################
OBJS := utils_app install

all:$(OBJS)

utils_app:
	make -C utils

install:
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	mkdir -p bin/
	mkdir -p mods/
	make -C utils install

clean:
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf bin/
	rm -rf mods/
	make -C utils clean
