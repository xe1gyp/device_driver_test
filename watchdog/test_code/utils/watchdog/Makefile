#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
CFLAGS += -I$(KDIR)/include -Wall

TARGETS = testwdtioctl testwdtwrite

all: $(TARGETS)

install:
	cp -v $(TARGETS) ../../bin

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vf *.o $(TARGETS)
