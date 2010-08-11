###########################################################################
#(C) Copyright Texas Instruments, 2007. All Rights Reserved.
#
# Name of the file: Makefile
#
# Location of the file:
#
# Brief description of the contents of the file:
#       Makefile for OS-Kernel tests
#
# Detailed description of the contents of the file:
# This Makefile is used to compile NCURSES / HTOP test programs.
#
# Author: Ricardo Perez Olivares (x0081762@ti.com)
#
# Created on: 27 Oct 2009
#
# Change Log:
#############################################################################

ifeq ($(SUITE_DIR),)
SUITE_DIR=$(PWD)
endif

DESTDIR := $(SUITE_DIR)/bin
#OBJS := utilities install
OBJS := install


HTOP_PWD=$(SUITE_DIR)
CPPFLAGS := "-I$(HTOP_PWD)/utils/ncurses-5.7/include/"
LDFLAGS := "-L$(HTOP_PWD)/utils/ncurses-5.7/lib/"
CONFIG_HOST := --host=arm-none-linux-gnueabi
NCURSESFLAGS := --with-shared --without-ada --disable-big-core --enable-termcap --enable-echo --enable-warnings

all: $(OBJS)

utilities:
	cd utils/ncurses-5.7; ./configure $(CONFIG_HOST) $(NCURSESFLAGS)
	make -C utils/ncurses-5.7/
	cd utils/htop-0.8.1/; CPPFLAGS=$(CPPFLAGS) LDFLAGS=$(LDFLAGS) ./configure $(CONFIG_HOST)
	make -C utils/htop-0.8.1/

install:
	mkdir -p $(DESTDIR)
	#cp utils/htop-0.8.1/htop $(DESTDIR)
	mkdir -p scripts/test
	mkdir -p scripts/tmp

clean:
	rm -rvf $(DESTDIR)
	#make -C utils/ncurses-5.7/ clean
	#make -C utils/htop-0.8.1/ clean
	rm -rf scripts/log*
	rm -rf scripts/tmp
	rm -rf scripts/test

