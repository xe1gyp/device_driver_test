#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc

USRDIR := ../../
CFLAGS := -I$(USRDIR)/include -Wall -mabi=aapcs-linux
LDFLAGS += -L$(USRDIR)/lib -lasound -lm -ldl -lpthread

TARGETS=test_mute test_gain

all: $(TARGETS)

install:
	cp -v $(TARGETS) ../../bin
	cp -v aplay-pr ../../bin

test_mute: test_mute.c

test_gain: test_gain.c

clean:
	rm -vf test_mute test_gain *.o
