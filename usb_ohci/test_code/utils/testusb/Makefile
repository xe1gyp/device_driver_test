#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

# arm-none-linux-gnueabi-gcc -Wall -g -lpthread -o testusb testusb.c

CC := $(CROSS_COMPILE)gcc
CFLAGS += -I$(KDIR)/include -Wall

TARGETS = testusb

all: $(TARGETS)

install:
	cp -v $(TARGETS) ../../bin

$(TARGETS):
	$(CC) $@.c -o $@ -lpthread

clean:
	rm -vf *.o $(TARGETS)
