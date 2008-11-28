###########################################################################
# (C) Copyright Texas Instruments, 2004. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file: /vobs/wtbu/OMAPSW_LT/kernel/device_drivers/camera/omap_camera_test
#
# Brief description of the contents of the file:
#	Makefile for Camera tests
#
# Detailed description of the contents of the file:
#	This file has generic rules, which can be expanded or compressed, on 
# an as needed basis, to help build test code. It has rules for building code 
# both in user space and in kernel space (mostly as modules in kernel space). 
# For test code that deals with user space alone, most of this could be
# drastically cut back.
#
#
# Author: Iqbal (iqbal2@india.ti.com)
#
# Created on: 26/07/2004
#
# Change Log:
############################################################################

#KERNELDIR = /mnt/hdb2/home/govindsk/bk-clearcase

# Directory that will be mounted as the target's root directory 
#DESTDIR = /mnt/hdb2/home/govindsk/target/build

# List of object/executable files needed to be built 
OBJS = dspset dspget dspopen test_log/*.o libtestlog.a

# List of object files that one of the others depends upon. Without this,
# there will be a circular dependency
DRVR_OBJS = 

# Variables related to the test/error log library - used in user space
# context only. TESTLOGPATH is a relative path again, within CVS and needs
# to be changed (until we have a copied-out release tree) for each Makefile.
# It is assumed here, that the object files related to the test log library
# are already built, for ARM.

TESTLOGPATH = test_log
TESTLOGOBJS := $(TESTLOGPATH)/testlog.o $(TESTLOGPATH)/list.o
TESTLOGINCLUDES := $(TESTLOGPATH)/list.h $(TESTLOGPATH)/testlog.h

# Variables for compilation

# This variable is for building in kernel mode
DEFS = -DLINUX -DMODULE -D__KERNEL__ -D__LINUX_ARMARCH=4
WARN = -Wall -Wstrict-prototypes
MISC = -fno-builtin -nostdinc
OPTIM = -O2

# With the common build base, if we move away from the arm tools packaged with
# MVL CEE, then the next set of variables needs to be updated. This also
# assumes that your PATH variable has the "bin" directory in it, where these
# tools reside
INCL = -I$(KERNELDIR)/include
#INCL += -I$(DESTDIR)/usr/lib/gcc-lib/arm-linux/3.3.1/include -I ../../harness/log
#INCL += -I$(TESTLOGPATH)
MODVERSIONS = $(shell if [ -e $(KERNELDIR)/include/linux/modversions.h ]; then echo "-DMODVERSIONS -include $(KERNELDIR)/include/linux/modversions.h"; fi)
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
# Include libraries here (in the override LIBDIRS line), that the tests 
# need, in order to build in user space
LIBFLAGS = -lc -g
override LIBFLAGS += 

# Used to build code in kernel mode - test code to be built as modules, 
# especially
CFLAGS = $(WARN) 
#override CFLAGS += $(DEFS) $(INCL) $(MODVERSIONS) $(MISC) $(OPTIM)

# This variable includes all the execuatbles and object files that need to
# be built, at the end of the "make" command or equivalent. 
all: libtestlog.a dspget dspset dspopen

install:
	cp dspget dspset dspopen libtestlog.a ../

# Rule to build executables in user space

libtestlog.a : $(TESTLOGOBJS) Makefile
	$(AR) r libtestlog.a $(TESTLOGOBJS)
list.o : list.h list.c Makefile
	${CC} -c list.c
testlog.o : testlog.h testlog.c Makefile
	${CC} -c testlog.c

dspset : dspset.c Makefile 
	$(CC) -I$(TESTLOGPATH) -o $@ $< $(LIBFLAGS) $(TESTLOGOBJS)	
dspget : dspget.c Makefile
	$(CC) -I$(TESTLOGPATH) -o $@ $< $(LIBFLAGS) $(TESTLOGOBJS)
dspopen : dspopen.c Makefile
	$(CC) -I$(TESTLOGPATH) -o $@ $< $(LIBFLAGS) $(TESTLOGOBJS)
clean:
	- rm -f $(OBJS) *.*~ ../dspget ../dspopen ../libtestlog.a ../dspset
