#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
CFLAGS += -Wall

TARGETS = watchdog_keepalive \
	watchdog_status_boot \
	watchdog_status \
	watchdog_support \
	watchdog_timeout_get \
	watchdog_timeout_set \
	watchdog_keepalive_smp \
	watchdog_write \

all: $(TARGETS)

install:
	cp -v $(TARGETS) $(SUITE_DIR)/binaries
#lpthread is needed for smp files only
$(TARGETS):
	$(CC) $(CFLAGS) -D_GNU_SOURCE -lpthread $@.c -o $@

clean:
	rm -vf *.o $(TARGETS)
