#/* ================================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found 
#  in the license agreement under which this software has been supplied.
# ================================================================================ */

ifeq ($(SUITE_DIR),)
SUITE_DIR=$(PWD)
endif

DESTDIR := $(SUITE_DIR)/modules
OBJS := lmbench3_app llcbench_apps install

all: $(OBJS)

lmbench3_app:
	make -C lmbench3

llcbench_apps:
	make -C llcbench cache-bench

install:
	mkdir -p $(DESTDIR)
	mkdir -p bin/
	mkdir -p bin/lmbench
	cp -rf lmbench3/bin/ bin/lmbench/
	cp -rf lmbench3/scripts/ bin/lmbench/
	mkdir -p bin/cachebench
	cp -rf llcbench/cachebench/cachebench bin/cachebench

clean:
	make -C lmbench3 clean
	make -C llcbench clean
	rm -rf bin/
	rm -rvf $(DESTDIR)
