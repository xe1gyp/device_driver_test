###########################################################################
# (C) Copyright Texas Instruments, 2009. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for LMBENCH 
#
# Detailed description of the contents of the file:
# This Makefile is used to compile LMBENCH test programs.
#
# Author: Ricardo Perez Olivares (x0081762@ti.com)
#
# Created on: 02 Oct 2009
#
# Change Log:
############################################################################

OBJS	:= utilities install

all: $(OBJS)

utilities:
	make OS=armv7l-linux-gnu CC=arm-none-linux-gnueabi-gcc -C utils/lmbench-3.0-a9/

install:
	mkdir -p scripts/test
	mkdir -p scripts/tmp

clean:
	make -C utils/lmbench-3.0-a9/ clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
