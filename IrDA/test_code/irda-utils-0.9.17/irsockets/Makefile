######################################################################
##                
## Filename:      Makefile
## Version:       
## Description:   Makefile for irsockets test programs
## Status:        Experimental.
## Author:        Dag Brattli <dagb@cs.uit.no>
## Created at:    Thu Feb 19 00:10:23 1998
## Modified at:   Sat Dec 25 16:13:54 1999
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

RM_CMD = $(RM) *.BAK *.bak *.o *.so ,* *~ *.a *.orig *.rej

INCLUDES  = $(SYS_INCLUDES) -I../include
LIBRARIES = $(SYS_LIBRARIES)
LIBPATH   = $(SYS_LIBPATH)

#
# System stuff
#
SYS_INCLUDES = -I/usr/include -I/usr/src/linux/include
SYS_LIBRARIES = 
SYS_LIBPATH = -L/usr/lib -L/usr/local/lib 

CFLAGS= $(SYS_INCLUDES) $(RPM_OPT_FLAGS) -O2 -Wall -Wstrict-prototypes

# 
# Files to make 
# 
PROG_SOCKET=irprintf irprintfx irscanf irscanfx irdaspray
PROG_EXTRA=ias_query send_ultra recv_ultra

all:: $(PROG_SOCKET) $(PROG_EXTRA)

%: %.o
	$(CC) $(XCFLAGS) -o $@ $^ $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

irprintf: irprintf.o
irprintfx: irprintfx.o
irscanf: irscanf.o
irscanfx: irscanfx.o
irdaspray: irdaspray.o
ias_query: ias_query.o
send_ultra: send_ultra.o
recv_ultra: recv_ultra.o

clean::
	$(RM_CMD) 

realclean::
	$(RM_CMD) 
	$(RM) $(PROG_SOCKET) $(PROG_EXTRA)

distclean:: realclean

depend::
	makedepend -s "# DO NOT DELETE" -- $(INCLUDES) -- $(SRCS)

# DO NOT DELETE
