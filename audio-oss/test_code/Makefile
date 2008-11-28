###########################################################################
# (C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:  
#
# Brief description of the contents of the file:
#       Makefile for Audio tests 
#
# Detailed description of the contents of the file:
# This Makefile is used to compile Audio test programs.  
#
# Author: Iqbal (iqbal@ti.com)
#
# Created on: 04/25/2007
#
# Change Log:
############################################################################
DESTDIR := ${PWD}/bin
OBJS := audio_app install

all:$(OBJS)

audio_app:
	make -C audio
install:
	mkdir -p $(DESTDIR)
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	mkdir -p scripts/raw_sin
	make -C audio install
clean:
	make -C audio clean
	rm -rvf $(DESTDIR)
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rf scripts/raw_sin


