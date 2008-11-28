/*********************************************************************
 *                
 * Filename:      irkbd.h
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Wed Mar 24 11:31:10 1999
 * Modified at:   Mon May  3 11:27:30 1999
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

#ifndef IRKBD_H
#define IRKBD_H

#include <sys/time.h>

/* Some commands */
#define IRKBD_CMD_INIT_KBD   0xfe
#define IRKBD_CMD_INIT_MOUSE 0xff
#define IRKBD_CMD_ENABLE     0x41
#define IRKBD_CMD_LED        0x31
#define IRKBD_CMD_KDB_SPEED  0x33

/* Some responses */
#define IRKBD_RSP_KBDOK      0x11
#define IRKBD_RSP_KBDERR     0x12
#define IRKBD_RSP_MSOK       0x21
#define IRKBD_RSP_MSERR      0x22
#define IRKBD_RSP_LEDOK      0x31
#define IRKBD_RSP_KBDSPEEDOK 0x33
#define IRKBD_RSP_RSPN41     0x41

#define IRKBD_RATE      15000   /* Polling rate, should be 15 ms */
#define IRKBD_IDLE_RATE 100000  /* 100 ms */
#define IRKBD_TIMEOUT   1       /* 1000 ms */

#define SUBFRAME_MASK     0xc0
#define SUBFRAME_MOUSE    0x80
#define SUBFRAME_MOUSE2   0xc0
#define SUBFRAME_KEYBOARD 0x40
#define SUBFRAME_RESPONSE 0x00

#define IRKBD_BUF_SIZE 4096 /* Must be power of 2! */

enum {
	IRKBD_IDLE,       /* Not connected */
	IRKBD_INIT_KBD,   /* Initializing keyboard */
	IRKBD_INIT_MOUSE, /* Initializing mouse */
	IRKBD_READY,
	IRKBD_POLLING,    /* Polling device */
};

/* Main structure */
struct irkbd_cb {
	int state;

	int sock;
	int fifo;
	int fd;

	int connected;

	__u32 saddr;        /* my local address */
	__u32 daddr;        /* peer address */

	int mtu_tx;

	struct itimerval watchdog_timer;

	__u8 frame[IRKBD_BUF_SIZE]; /* Buffer for received data */
	int head;
	int tail;
};

#endif /* IRKBD_H */
