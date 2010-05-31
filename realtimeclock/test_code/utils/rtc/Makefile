# /* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif

CC := $(CROSS_COMPILE)gcc
CFLAGS += -g -O -Wall -D_GNU_SOURCE  -lpthread -I.. -pthread

TARGETS := alarm_get alarm_get_event alarm_reset alarm_set per_int per_int_get per_int_get1 per_int_get2 per_int_off per_int_on time_set time_get smp_date rtc_read alarm_read

all: $(TARGETS)

install:
	cp -vf $(TARGETS) $(SUITE_DIR)/bin

$(TARGETS): 
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -vf $(TARGETS)
