######################################################################
##                
## Filename:      Makefile
## Version:       
## Description:   Makefile for Linux IrDA Manager
## Status:        Experimental.
## Author:        Dag Brattli <dagb@cs.uit.no>
## Created at:    Thu Feb 19 00:10:23 1998
## Modified at:   Tue Jan 25 09:54:38 2000
## Modified by:   Dag Brattli <dagb@cs.uit.no>
## 
## $Id: Makefile,v 1.9 2004/04/01 20:22:22 claudiuc Exp $
##
##     Copyright (c) 1998-2000 Dag Brattli, All Rights Reserved.
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

DIRS = irattach irdaping etc man irnetd psion tekram findchip irdadump 
CFLAGS= -O2 -W -Wall

all:
	echo "*** Making sure some directories exists ***"
	mkdir -p $(RPM_BUILD_ROOT)/usr/sbin
	mkdir -p $(RPM_BUILD_ROOT)/usr/bin $(RPM_BUILD_ROOT)/usr/X11R6/bin
## Red-Hat specific, not available on Debian
##	mkdir -p $(RPM_BUILD_ROOT)/etc/rc.d/init.d
	cd irdadump;CFLAGS="$(CFLAGS)" CXXFLAGS="$(CFLAGS)" ./autogen.sh --prefix=$(RPM_BUILD_ROOT)/usr
	for i in $(DIRS); do \
                cd $$i;make all; cd ..; \
        done

install:
	for i in $(DIRS); do \
		cd $$i;make install; cd ..; \
	done

clean:
	set -e ; for d in $(DIRS) ; do $(MAKE) -C $$d $@ ; done

distclean:
	set -e ; for d in $(DIRS) ; do $(MAKE) -C $$d $@ ; done

