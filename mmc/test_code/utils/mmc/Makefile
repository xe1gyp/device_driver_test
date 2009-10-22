#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
TARGETS = mmc_test_dev0 mmc_test_dev1

all: $(TARGETS)	

install:
	cp $(TARGETS) ../../bin

$(TARGETS): mmc_test_dev0.c mmc_test_dev1.c
	$(CC) $(CFLAGS) $@.c list.c testlog.c -Wall -o $@

clean:
	rm -f $(TARGETS)
