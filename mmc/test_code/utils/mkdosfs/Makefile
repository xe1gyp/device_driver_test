#
# Filename:	Makefile
# Author:	Dave Hudson
# Version:	0.3
# Started:	24th August 1993
# Last Updated:	8th May 2007
# Updated by:   H. Peter Anvin <chaffee@bmrc.berkeley.edu>
#		Misael Lopez Cruz
#
# Description:	Makefile for the "mkdosfs" command
#

CC := $(CROSS_COMPILE)gcc

all: mkdosfs

install:
	cp -rf mkdosfs ../../bin

mkdosfs: mkdosfs.o
	$(CC) $(CFLAGS) mkdosfs.c -o mkdosfs

clean:
	rm -f *.s *.o *.a *~ core mkdosfs
