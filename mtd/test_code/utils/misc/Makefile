#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
TARGETS := log differ genfile fileio

all: $(TARGETS)

install: log
	cp -vf $(TARGETS) ../../bin

log: list.c testlog.c testtestlog5.c
	$(CC) -o log list.c testlog.c testtestlog5.c

differ: differ.c
	$(CC) -o differ differ.c

genfile: genfile.c
	$(CC) -o genfile genfile.c

fileio: fileio.c
	$(CC) -o fileio fileio.c

clean:
	rm -vf $(TARGETS)
