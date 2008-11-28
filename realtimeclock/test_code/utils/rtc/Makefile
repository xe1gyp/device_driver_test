#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

CC := $(CROSS_COMPILE)gcc
CFLAGS=-g -O -Wall -I.. 

TARGETS = settime gettime getperint1 getperint2 perintoff perinton alarm_set getalarm_event alarm_reset

all: $(TARGETS)

install:
	cp -vf $(TARGETS) ../../bin

$(TARGETS): 
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -vf $(TARGETS)
