#===========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
#============================================================================

CC := $(CROSS_COMPILE)gcc
CFLAGS+=-march=armv7-a -mfpu=vfpv3 -mfloat-abi=softfp

ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif

TARGETS = vfptest

all: $(TARGETS)

install:
	cp -v $(TARGETS) $(SUITE_DIR)/bin

$(TARGETS):
	$(CC) $(CFLAGS) -D_GNU_SOURCE $@.c -o $@

clean:
	rm -vf *.o $(TARGETS)
