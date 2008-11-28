######################################################################
##                
## Filename:      Makefile
## Version:       
## Description:   Makefile for irnetd
## Status:        Experimental.
## Author:        Dag Brattli <dagb@cs.uit.no>
## Created at:    Thu Feb 19 00:10:23 1998
## Modified at:   Sat Dec 25 16:13:54 1999
## Modified by:   Dag Brattli <dagb@cs.uit.no>
## 
## $Id: Makefile,v 1.4 2004/04/01 20:22:30 claudiuc Exp $
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

RM_CMD = $(RM) *.BAK *.bak *.o *.so ,* *~ *.a *.orig *.rej

INCLUDES  = $(SYS_INCLUDES) -I../include
LIBRARIES = $(SYS_LIBRARIES)
LIBPATH   = $(SYS_LIBPATH)

CFLAGS= $(RPM_OPT_FLAGS) -O2 -Wall -Wstrict-prototypes

# 
# Files to make 
# 
PROG_SOCKET=irnetd

all:: $(PROG_SOCKET)

%: %.o
	$(CC) $(XCFLAGS) -o $@ $^ $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

irnetd: irnetd.o

clean::
	$(RM_CMD) 

realclean::
	$(RM_CMD) 
	$(RM) $(PROG_SOCKET) $(PROG_EXTRA)

distclean:: realclean

depend::
	makedepend -s "# DO NOT DELETE" -- $(INCLUDES) -- $(SRCS)

# DO NOT DELETE
