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
O := -O2 -fomit-frame-pointer
CFLAGS += $(O) -Wall
LDFLAGS := -s

all: mkdosfs

install:
	cp -rf mkdosfs ../../bin

mkdosfs: mkdosfs.o
	$(CC) $(LDFLAGS) mkdosfs.o -o mkdosfs

mkdosfs.o: mkdosfs.c
	$(CC) -c $(CLFAGS) mkdosfs.c

clean:
	rm -f *.s *.o *.a *~ core mkdosfs
