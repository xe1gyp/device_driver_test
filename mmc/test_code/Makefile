###########################################################################
# (C) Copyright Texas Instruments. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for mmc tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile mmc test programs.
#
############################################################################

DESTDIR := binaries
OBJS := utilities install

all: $(OBJS)

utilities:
	make -C utils

install:
	mkdir -p $(DESTDIR)
	mkdir -p scripts/test
	mkdir -p scripts/tmp
	make -C utils install

clean:
	make -C utils clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test
	rm -rvf $(DESTDIR)
