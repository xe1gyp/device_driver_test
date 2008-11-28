# ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ================================================================================


CC=$(CROSS_COMPILE)gcc

BINARIES:=dhry

all: 
	$(CC) -c -DTIME -static -march=armv6 -DHZ= -O dhry_1.c
	$(CC) -c -DTIME -static -march=armv6 -DHZ= -O dhry_2.c
	$(CC) -o ./dhry -DTIME -static -march=armv6 -O  ./dhry_1.o ./dhry_2.o  

install:
	cp $(BINARIES) ../../bin/

clean:
	rm -vf ${BINARIES}
