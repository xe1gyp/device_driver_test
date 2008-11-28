/*********************************************************************
 *                
 * Filename:      capture.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Jan Kiszka
 * Created at:    Thu Jul 17 16:32:35 PDT 2003
 * Modified at:   Thu Jul 17 16:32:35 PDT 2003
 * Modified by:   Jean Tourrilhes
 * 
 *     Copyright (c) 1999 Jan Kiszka, All Rights Reserved.
 *     Copyright (c) 2003 Jean Tourrilhes, All Rights Reserved.
 *      
 *     This program is free software; you can redistribute it and/or 
 *     modify it under the terms of the GNU General Public License as 
 *     published by the Free Software Foundation; either version 2 of 
 *     the License, or (at your option) any later version.
 *  
 *     Neither Dag Brattli nor University of Tromsø admit liability nor
 *     provide warranty for any of this software. This material is 
 *     provided "AS-IS" and at no charge.
 *
 ********************************************************************/

#ifndef CAPTURE_H
#define CAPTURE_H

#include "netbuf.h"

struct sockaddr_ll;

extern int capwrite_open(char *	capfilename);
extern void capwrite_close(int capturefile);
extern int capwrite_init(int capturefile);
extern int capwrite_dump(int capturefile,
			GNetBuf *frame_buf,
			int len,
			struct sockaddr_ll *from,
			struct timeval *curr_time);
extern int capread_open(char *	capfilename);
extern void capread_close(int capturefile);
extern int capread_check(int capturefile);
extern int capread_get(int capturefile,
		       GNetBuf *frame_buf,
		       int *plen,
		       int *pdir,
		       int *pprot,
		       struct timeval *curr_time);

#endif



