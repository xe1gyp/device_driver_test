#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */
ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif

BIN_DIR:=$(SUITE_DIR)/bin

CC := $(CROSS_COMPILE)gcc

TARGETS = evtest

all: $(TARGETS)

install:
	@cp -v $(TARGETS) $(SUITE_DIR)/bin

clean:
	rm -f *.o $(TARGETS)
