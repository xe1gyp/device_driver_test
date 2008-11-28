######################################################################
##                
## Filename:      Makefile
## Version:       
## Description:   Makefile for Linux IrDA Manager
## Status:        Experimental.
## Author:        Dag Brattli <dagb@cs.uit.no>
## Created at:    Thu Feb 19 00:10:23 1998
## Modified at:   Wed Oct  6 20:22:18 1999
## Modified by:   Dag Brattli <dagb@cs.uit.no>
## 
## $Id: Makefile,v 1.10 2004/04/01 20:22:24 claudiuc Exp $
##
##     Copyright (c) 1998-1999 Dag Brattli, All Rights Reserved.
##      
##     This program is free software; you can redistribute it and/or 
##     modify it under the terms of the GNU General Public License as 
##     published by the Free Software Foundation; either version 2 of 
##     the License, or (at your option) any later version.
##  
##     Neither Dag Brattli nor University of Tromsø admit liability nor
##     provide warranty for any of this software. This material is 
##     provided "AS-IS" and at no charge.
##     
######################################################################

CC = gcc
LD = ld
RM = rm -f

RM_CMD = $(RM) *.BAK *.bak *.o ,* *~ *.a

INCLUDES  = $(SYS_INCLUDES) -I../include/
LIBRARIES = $(SYS_LIBRARIES)
LIBPATH   = $(SYS_LIBPATH)

#
# System stuff
#
SYS_INCLUDES = -I/usr/include
SYS_LIBRARIES = 
SYS_LIBPATH = -L/usr/lib -L/usr/local/lib 

CFLAGS= -O2 -W -Wall -Wstrict-prototypes -Wshadow $(SYS_INCLUDES)

# 
# Files to make 
# 
SRCS = util.c
OBJS = $(SRCS:.c=.o)
TARGETS = irattach dongle_attach

all: $(TARGETS)

irattach: irattach.o util.o
	$(CC) $(CFLAGS) irattach.o util.o -o $@

dongle_attach: dongle_attach.o
	$(CC) $(CFLAGS) dongle_attach.o -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

install: $(TARGETS)
	install irattach $(ROOT)/usr/sbin
	install dongle_attach $(ROOT)/usr/sbin

clean:
	$(RM_CMD)

distclean:
	$(RM_CMD)
	$(RM) irattach dongle_attach

tags:
	etags *.[hc] */*.[hc] */*/*.[hc]

depend:
	makedepend -s "# DO NOT DELETE" -- $(INCLUDES) -- $(SRCS)

# DO NOT DELETE
