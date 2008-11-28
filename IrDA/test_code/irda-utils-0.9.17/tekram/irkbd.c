/*********************************************************************
 *                
 * Filename:      irkbd.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Wed Mar 24 11:25:17 1999
 * Modified at:   Sat Sep  4 21:20:22 1999
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

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/kd.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <irda.h>

#include <asm/byteorder.h>

#include "irkbd.h"

static void irkbd_data_indication(int signo);
static inline void irkbd_start_watchdog_timer(int sec, int usec);

#ifndef AF_IRDA
#define AF_IRDA 23
#endif /* AF_IRDA */

#define MAX_DEVICES 10
#define FIFO_NAME "/dev/irkbd"

struct irkbd_cb irkbd;
struct irkbd_cb *self = &irkbd;

int power_save = 0;

/*
 * Function irkbd_init ()
 *
 *    Initialize IrKBD
 *
 */
static void irkbd_init(struct irkbd_cb *self)
{
	int fd;
	int oflags;

	memset(&irkbd, 0, sizeof(struct irkbd_cb));

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }
	
	/* Register for asynchronous notification */
	signal(SIGIO, &irkbd_data_indication);
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(0, F_GETFL);
	if (fcntl(fd, F_SETFL, oflags | FASYNC) < 0) {
		perror("fcntl");
		exit(-1);
	}
	self->sock = fd;

	/* Make fifo */
	if (mknod (FIFO_NAME, 0666 | S_IFIFO, 0)) {
		/* Just be happy if it's already there */
		if (errno != EEXIST) {
			perror("mknod");
			exit(-1);
		}
	}
	fd = open(FIFO_NAME, O_RDWR|O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		exit(-1);
	}
	self->fifo = fd;

	fd = open("/dev/console", O_RDWR);
	if (fd < 0)
		return;
	self->fd = fd;
	
	self->state = IRKBD_IDLE;
}

/*
 * Function irkbd_discover_devices (fd)
 *
 *    Try to discover some remote device(s) that we can connect to
 *
 */
static int irkbd_discover_devices(void)
{
	struct irda_device_list *list;
	unsigned char *buf;
	int len;
	int i;

	len = sizeof(struct irda_device_list) +
		sizeof(struct irda_device_info) * MAX_DEVICES;

	buf = malloc(len);
	list = (struct irda_device_list *) buf;
	
	if (getsockopt(self->sock, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) {
		perror("getsockopt");
		exit(-1);
	}

	if (len > 0) {
		printf("Discovered: (list len=%d)\n", list->len);

		for (i=0;i<list->len;i++) {
			printf("  name:  %s\n", list->dev[i].info);
			printf("  daddr: %08x\n", list->dev[i].daddr);
			printf("  saddr: %08x\n", list->dev[i].saddr);
			printf("\n");
			
			self->daddr = list->dev[i].daddr;
			self->saddr = list->dev[i].saddr;

			if (strcmp(list->dev[i].info, "IR-660") == 0) {
				printf("This one looks good!\n");
				return 0;
			}
		}
	}
	printf("Hmm, didn't find any! retry?\n");
	return -1;
}

/*
 * Function irkbd_connect_request (self)
 *
 *    Try to connect to remote device
 *
 */
static int irkbd_connect_request(void)
{
	struct sockaddr_irda peer;
	int mtu = 0;
	int len = sizeof(int);
	int ret;

	/*
	 *  Open TTP connection to remote device!
	 */
	ret = irkbd_discover_devices();
	if (ret == -1)
		return -1;

	/* Disable IrTTP SAR */
	if (setsockopt(self->sock, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, &mtu, 
		       sizeof(mtu))) 
	{
		perror("setsockopt");
		exit(-1);
	}

	peer.sir_family = AF_IRDA;
	strncpy(peer.sir_name, "IrKBMS", 25);
	peer.sir_addr = self->daddr;
	
	if (connect(self->sock, (struct sockaddr*) &peer, 
		    sizeof(struct sockaddr_irda))) {
		perror("connect");
		return -1;
	}

	/* Check what the IrLAP data size is */
	if (getsockopt(self->sock, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, 
		       (void *) &mtu, &len)) 
	{
		perror("getsockopt");
		exit(-1);
	}
	self->mtu_tx = mtu;
	/* printf(__FUNCTION__ "(), link mtu=%d\n", mtu); */

	self->connected = 1;

	return 0;
}

/*
 * Function irkbd_data_request (self, cmd)
 *
 *    Send command to device
 *
 */
static void irkbd_data_request(int cmd)
{
	char buf[1];
	int ret;

	switch (cmd) {
	case IRKBD_CMD_ENABLE:
		buf[0] = IRKBD_CMD_ENABLE;
		break;
	case IRKBD_CMD_INIT_KBD:
		buf[0] = IRKBD_CMD_INIT_KBD;
		break;
	case IRKBD_CMD_INIT_MOUSE:
		buf[0] = IRKBD_CMD_INIT_MOUSE;
		break;
	default:
		break;
	}
	ret = send(self->sock, buf, 1, 0);
	if (ret < 0)
		printf("Disconnected2!\n");
}
/*
 * Function irkbd_process_response (self, rsp)
 *
 *    Process response code
 *
 */
static inline void irkbd_handle_response(__u8 rsp)
{
	switch (rsp) {
	case IRKBD_RSP_RSPN41:
		if (power_save++ > 100)
			irkbd_start_watchdog_timer(0, IRKBD_IDLE_RATE); 
		else
			irkbd_start_watchdog_timer(0, IRKBD_RATE); 
		break;
	case IRKBD_RSP_KBDOK:
		printf("OK!\n");
				
		self->state = IRKBD_INIT_MOUSE;
		irkbd_start_watchdog_timer(0, IRKBD_RATE);
		break;
	case IRKBD_RSP_KBDERR:
		printf("Error!\n");
		self->state = IRKBD_INIT_MOUSE;
		irkbd_start_watchdog_timer(0, IRKBD_RATE);
		break;
	case IRKBD_RSP_MSOK:
		printf("OK!\n");

		self->state = IRKBD_READY;
		irkbd_start_watchdog_timer(0, 100000);
		break;
	case IRKBD_RSP_MSERR:
		printf("Error!\n");
		self->state = IRKBD_READY;
		irkbd_start_watchdog_timer(0, 100000);
		break;
	case IRKBD_RSP_LEDOK:
		break;
	case IRKBD_RSP_KBDSPEEDOK:
		break;
	default:
		printf(__FUNCTION__ "(), unknown response %02x\n", rsp);
		break;
	}
}

/*
 * Function irkbd_handle_mouse (self, scancode)
 *
 *    Handle mouse event. The scancodes follows the PS/2 mouse protocol. Half
 *    a protocol for half a mouse :-)
 */
static inline void irkbd_handle_mouse(__u8 scancode)
{	
	/* Write to fifo */
	write(self->fifo, &scancode, 1);
}

static inline void handle_scancode(__u8 scancode)
{
	/* Insert scancode into kernel */
	if (ioctl(self->fd,TIOCSTI, &scancode))
		perror("ioctl");	
}

/*
 * Function irkbd_data_indication (instance, sap, skb)
 *
 *    Received some mouse and/or keyboard data
 *
 */
static void irkbd_data_indication(int signo)
{
	__u8 *frame = self->frame;
	int actual;
	int i, n=0;
	__u8 len;
	__u8 sublen;
	__u8 subtype;

	actual = recv(self->sock, frame, 2048, 0);
	if (actual <= 0) {
		printf("Disconnected!\n");
		self->connected = 0;
	}
	/* Get lenght of frame */
	len = frame[n++];

	/* Parse frame */
	while (len > 0) {
		sublen  = frame[n] & ~SUBFRAME_MASK;  /* Len of subframe */
		subtype = frame[n++] & SUBFRAME_MASK; /* Type of subframe */
		len -= 1;

		switch (subtype) {
		case SUBFRAME_MOUSE:
		case SUBFRAME_MOUSE2:
			/* Mouse subframes contains 3 bytes pr. record */
			for (i=0;i<sublen*3;i++) {
				/* Process mouse scancode */
				irkbd_handle_mouse(frame[n++]);
			}
			len -= (sublen*3);
			power_save = 0;
			irkbd_start_watchdog_timer(0, IRKBD_RATE);
			break;
		case SUBFRAME_KEYBOARD:
			printf("Test!\n");
			for(i=0;i<sublen;i++) {
				/* Process keyboard scancode */
				handle_scancode(frame[n++]);
			}
			len -= sublen;
			power_save = 0;
			irkbd_start_watchdog_timer(0, IRKBD_RATE);
			break;
		case SUBFRAME_RESPONSE:
			for(i=0;i<sublen;i++)
				irkbd_handle_response(frame[n++]);
			len -= sublen;
			break;
		default:
			printf("Test2\n");
			len -= sublen;
			break;
		}
	}
}

/*
 * Function irkbd_start_watchdog_timer (self)
 *
 *    
 *
 */
static inline void irkbd_start_watchdog_timer(int sec, int usec)
{
     	self->watchdog_timer.it_value.tv_sec = sec;
	self->watchdog_timer.it_value.tv_usec = usec;
	
	setitimer(ITIMER_REAL, &self->watchdog_timer, NULL);
}

/*
 * Function irkbd_watchdog_timer_expired (data)
 *
 *    
 *
 */
void irkbd_watchdog_timer_expired(int signo)
{
	switch (self->state) {
	case IRKBD_POLLING: /* Optimize the common case */
		irkbd_data_request(IRKBD_CMD_ENABLE);
		irkbd_start_watchdog_timer(IRKBD_TIMEOUT, 0);
		break;
	case IRKBD_IDLE:
		break;
	case IRKBD_INIT_KBD:
		printf("Initializing keyboard: ");
		irkbd_data_request(IRKBD_CMD_INIT_KBD);
		break;
	case IRKBD_INIT_MOUSE:
		printf("Initializing mouse: ");
		irkbd_data_request(IRKBD_CMD_INIT_MOUSE);
		break;
	case IRKBD_READY:
		printf("Running ...\n");
		irkbd_data_request(IRKBD_CMD_ENABLE);
		self->state = IRKBD_POLLING;
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	int err;

	irkbd_init(self);

	signal(SIGALRM, irkbd_watchdog_timer_expired);

	for (;;) {
		/* Try connect */
		err = irkbd_connect_request();
		if (err) {
			sleep(5);
			continue;
		}
		
		self->state = IRKBD_INIT_KBD;
		self->state = IRKBD_INIT_MOUSE;
		irkbd_start_watchdog_timer(IRKBD_TIMEOUT, 0);
		
		while (self->connected)
			sleep(1);

		printf("Disconnected!\n");
	}
	return 0;
}

