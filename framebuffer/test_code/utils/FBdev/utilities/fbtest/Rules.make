
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
HOSTCC = gcc

IFLAGS = -I$(TOPDIR)/include
#DFLAGS = -g
OFLAGS = -O3 -fomit-frame-pointer
CFLAGS = -Wall $(IFLAGS) $(DFLAGS) $(OFLAGS)

SRCS += $(wildcard *.c)
OBJS += $(subst .c,.o,$(SRCS))
HDRS += $(wildcard *.h)
HDRS += $(wildcard $(TOPDIR)/include/*.h)

.PHONY:		all all_rec clean clean_rec


all:		.depend all_rec $(TARGET) $(O_TARGET) $(HOST_TARGET)

all_rec:
ifdef SUBDIRS
		cp -f ../images-bak/penguin.c images/penguin.c
		for i in $(SUBDIRS); do make -C $$i all; done
endif

$(TARGET):	$(OBJS)
		$(CC) -o $(TARGET) $(filter $(OBJS), $^) $(LIBS)

$(O_TARGET):	$(OBJS)
		$(LD) -r -o $(O_TARGET) $(filter $(OBJS), $^)

$(HOST_TARGET):	$(SRCS)
		$(HOSTCC) -o $(HOST_TARGET) $(CFLAGS) $(SRCS) $(LIBS)

install:
		cp arm-none-linux-gnueabi-fbtest ../../../../bin

clean::		clean_rec
		$(RM) $(TARGET) $(HOST_TARGET) $(OBJS) .depend

clean_rec:
ifdef SUBDIRS
		for i in $(SUBDIRS); do make -C $$i clean; done
endif


%.o:		%.c
		$(CC) -c $(CFLAGS) -o $@ $<


ifeq ($(HOST_TARGET),)
.depend:	$(SRCS) $(HDRS)
		$(CC) -M $(CFLAGS) $(SRCS) > .depend
else
.depend:	$(SRCS) $(HDRS)
		$(HOSTCC) -M $(CFLAGS) $(SRCS) > .depend
endif

ifneq ($(wildcard .depend),)
include .depend
endif

