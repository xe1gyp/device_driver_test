/*********************************************************************
 *                
 * Filename:      obex.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Mon Apr 12 10:32:56 1999
 * Modified at:   Wed Apr 21 16:01:44 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999 Dag Brattli, All Rights Reserved.
 *     
 *     This program is free software; you can redistribute it and/or 
 *     modify it under the terms of the GNU General Public License as 
 *     published by the Free Software Foundation; either version 2 of 
 *     the License, or (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License 
 *     along with this program; if not, write to the Free Software 
 *     Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *     MA 02111-1307 USA
 *     
 ********************************************************************/

#ifndef OBEX_H
#define OBEX_H

/* OBEX Commands */
#define OBEX_CONNECT       0x00
#define OBEX_DISCONNECT    0x01
#define OBEX_PUT           0x02
#define OBEX_GET           0x03
#define OBEX_COMMAND       0x04
#define OBEX_SETPATH       0x05
#define OBEX_ABORT         0x7f

/* OBEX Responses */
#define	OBEX_CONTINUE              0x10
#define OBEX_SWITCH_PRO            0x11
#define OBEX_SUCCESS               0x20
#define OBEX_CREATED               0x21
#define OBEX_ACCEPTED              0x22
#define OBEX_BAD_REQUEST	   0x40
#define OBEX_FORBIDDEN             0x43
#define OBEX_CONFLICT              0x49
#define OBEX_INTERNAL_SERVER_ERROR 0x50
#define OBEX_NOT_IMPLEMENTED       0x51
#define OBEX_DATABASE_FULL         0x60
#define OBEX_DATABASE_LOCKED       0x61

#define HEADER_COUNT       0xc0
#define HEADER_NAME        0x01 /* - */
#define HEADER_ANAME       0x31 /* Same as HEADER_NAME, but in ASCII */
#define HEADER_TYPE        0x42 /* - */
#define HEADER_TIME        0x44
#define HEADER_LENGTH      0xc3 /* - */
#define HEADER_DESCRIPTION 0x05 /* - */
#define HEADER_TARGET	   0x46 /* - */
#define HEADER_BODY        0x48 /* - */
#define HEADER_BODY_END    0x49 /* - */

#define OBEX_HI_MASK       0xc0
#define OBEX_UNICODE       0x00
#define OBEX_BYTE_STREAM   0x40
#define OBEX_BYTE          0x80
#define OBEX_INT           0xc0

#define OBEX_FINAL         0x80

/* Connect header */
struct obex_connect_frame {
	guint8  opcode;
	guint16 len;
	guint8  version;
	guint8  flags;
	guint16 mtu;
} __attribute__((packed));

/* Minimal Obex header */
struct obex_minimal_frame {
	guint8  opcode;
	guint16 len;
} __attribute__((packed));

#endif
