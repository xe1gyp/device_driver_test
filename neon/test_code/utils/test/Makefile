#/* ===========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ============================================================================*/

CC := $(CROSS_COMPILE)gcc
CFLAGS=-march=armv7-a -mfpu=neon -mfloat-abi=softfp

TARGETS = neontest

all: $(TARGETS)

install:
	cp -vf $(TARGETS) ../../bin

$(TARGETS):
	$(CC) $@.S $(CFLAGS) -o $@

clean:
	rm -vf $(TARGETS)
