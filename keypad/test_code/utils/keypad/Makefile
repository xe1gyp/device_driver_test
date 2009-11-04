#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
CFLAGS+=-g -O -Wall -I..

TARGETS = dev_name openclose testkeypad

all: $(TARGETS)

install:
	cp -vf $(TARGETS) ../../bin

$(TARGETS):
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -vf $(TARGETS)
