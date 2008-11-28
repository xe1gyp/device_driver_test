######################################################################
##                
## Filename:      Makefile
## Version:       0.1
## Description:   Hardware probe for Infrared Communications Controllers
## Status:        Experimental.
## Author:        Jonathan Hudson <jrhudson@bigfoot.com>
## Created at:    Thu Jan  1 09:54:08 2000
## Modified at:   Tue Jan 25 10:01:42 2000
## Modified by:   Dag Brattli <dagb@cs.uit.no>
## 
##     Copyright (c) 2000 Jonathan Hudson <jrhudson@bigfoot.com>,
##     Copyright (c) 2000 Dag Brattli <dagb@cs.uit.no>, 
##     All Rights Reserved.
##     
##     This program is free software; you can redistribute it and/or 
##     modify it under the terms of the GNU General Public License as 
##     published by the Free Software Foundation; either version 2 of 
##     the License, or (at your option) any later version.
## 
##     This program is distributed in the hope that it will be useful,
##     but WITHOUT ANY WARRANTY; without even the implied warranty of
##     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
##     GNU General Public License for more details.
## 
##     You should have received a copy of the GNU General Public License 
##     along with this program; if not, write to the Free Software 
##     Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
##     MA 02111-1307 USA
##     
######################################################################

CC      = gcc
CFLAGS= $(RPM_OPT_FLAGS) $(SYS_INCLUDES) -O2 -Wall -Wstrict-prototypes

SRCS = findchip.c smc.c nsc.c winbond.c
OBJS = $(SRCS:.c=.o)

all: findchip

findchip: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *~ #*

distclean:
	rm -f findchip gfindchip *.o *~ #*

install: findchip
	cp findchip $(ROOT)/usr/sbin/

gfindchip: gfindchip.c 
	$(CC) $(CFLAGS) `gtk-config --cflags`  $< -o $@ `gtk-config --libs`

