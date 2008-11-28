######################################################################
##                
## Filename:      Makefile
## Version:       
## Description:   Makefile for irdaping
## Status:        Experimental.
## Author:        Dag Brattli <dagb@cs.uit.no>
## Created at:    Thu Feb 19 00:10:23 1998
## Modified at:   Tue Sep  7 21:01:30 1999
## Modified by:   Dag Brattli <dagb@cs.uit.no>
## 
## $Id: Makefile,v 1.8 2004/04/01 20:22:31 claudiuc Exp $
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

INCLUDES  = $(SYS_INCLUDES)
LIBRARIES = $(SYS_LIBRARIES)
LIBPATH   = $(SYS_LIBPATH)

#
# System stuff
#
SYS_INCLUDES = -I/usr/src/linux/include -I/usr/include -I../include
SYS_LIBRARIES = 
SYS_LIBPATH = -L/usr/lib -L/usr/local/lib 

CFLAGS= $(RPM_OPT_FLAGS) $(SYS_INCLUDES) -Wall -Wstrict-prototypes

# 
# Files to make 
# 
SRCS = irkbd.c
OBJS = $(SRCS:.c=.o)

all:: irkbd

irkbd: $(OBJS)
	$(CC) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

install: all
	install irkbd $(ROOT)/usr/bin

clean::
	$(RM_CMD)

distclean::
	$(RM_CMD)
	$(RM) irkbd

depend::
	makedepend -s "# DO NOT DELETE" -- $(INCLUDES) -- $(SRCS)

# DO NOT DELETE
