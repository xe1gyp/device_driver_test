/*********************************************************************
 *
 * Filename:      irdadump.c
 * Version:       0.6.1
 * Description:   irdadump sniffs IrDA frames, and is inspired by tcpdump
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Sun Oct  4 20:33:05 1998
 * Modified at:   Wed Jan 19 11:03:32 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 *
 *     Copyright (c) 1998-2000 Dag Brattli, All Rights Reserved.
 *     Copyright (c) 2002-2003 Jean Tourrilhes, All Rights Reserved.
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

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <net/if_arp.h>
#include <net/if_packet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>		/* htons */

#include <netpacket/packet.h>

#include <stdint.h>
#include <string.h>		/* strncpy */
#include <stdio.h>		/* perror */
#include <stdlib.h>		/* exit */
#include <irda.h>

#include "capture.h"
#include "irdadump.h"

int config_print_diff = 0;
int config_dump_frame = 0;
int config_print_irlap = 1;
int config_print_irlmp = 1;
int config_print_lost_frames = 0;
int config_snaplen = 32;
int config_dump_bytes = 0;
int config_snapcols = 16;
int config_force_ttp = 0;
int config_capturewrite = -1;
int config_captureread = -1;

int verbose = 0;
int ifindex = 0;

GNetBuf *frame_buf;
struct timeval time1, time2;
struct timeval *curr_time, *prev_time, *tmp_time;
int fd = -1;

/*
 * Function print_time (tvp)
 *
 *    Print current time
 *
 */
inline void print_time(const struct timeval *timev, GString *str)
{
        int s;

	s = (timev->tv_sec) % 86400;
	/* Modulo is not a proper modulo for negative ints */
	if(s < 0)
		s += 86400;
	g_string_sprintfa(str, "%02d:%02d:%02d.%06u ",
			  s / 3600, (s % 3600) / 60,
			  s % 60, (u_int32_t) timev->tv_usec);
}

/*
 * Function print_diff_time (time, prev_time)
 *
 *    Print the difference in time between this frame and the previous one
 *
 */
inline void print_diff_time(struct timeval *timev, struct timeval *prev_timev,
			    GString *str)
{
	float diff;

	if (prev_timev->tv_usec > timev->tv_usec) {
		timev->tv_usec += 1000000;
		timev->tv_sec--;
	}
	prev_timev->tv_usec = timev->tv_usec - prev_timev->tv_usec;
	prev_timev->tv_sec  = timev->tv_sec - prev_timev->tv_sec;

	diff = ((float) prev_timev->tv_sec * 1000000 + prev_timev->tv_usec)
		/ 1000.0;

	g_string_sprintfa(str, "(%07.2f ms) ", diff);
}

/*
 * Function irdadump_init ()
 *
 *
 *
 */
int irdadump_init(char *ifdev)
{
	struct ifreq ifr;

	/* Zero state of parser */
	irlap_init();

	curr_time = &time1;
	prev_time = &time2;

	/* Get time, so the first time diff is right */
	gettimeofday(prev_time, (struct timezone*) 0);


        /*
	 * Create socket, we must use SOCK_DGRAM to get the link level header
	 * that we are interested in
	 */
	fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	if (fd < 0) {
		return fd;
        }

	if (ifdev) {
		strncpy(ifr.ifr_name, ifdev, IFNAMSIZ);
		if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
			perror("ioctl");
			exit(-1);
		}
		ifindex = ifr.ifr_ifindex;
	}

	frame_buf = g_netbuf_new(MAX_FRAME_SIZE);

	return fd;
}

/*
 * Function parse_irda_frame (buf, len)
 *
 *    Do the various kind of parsing we need on this frame.
 *
 */
void parse_irda_frame(int type, GNetBuf *buf, int len, GString *str)
{
	/* Captured time (more or less always Rx/Tx time) */
 	print_time(curr_time, str);

	if (config_print_diff) {
		print_diff_time(curr_time, prev_time, str);

		/* Swap */
		tmp_time = prev_time;
		prev_time = curr_time;
		curr_time = tmp_time;
	}

	/* Full IrDA decoding of Frame */
	if (config_print_irlap)
		parse_irlap_frame(type, buf, str);
	/* Overall size */
        g_string_sprintfa(str, "(%d) ", len);

	/* Byte dumps */
	if (config_dump_frame) {
		int i, maxlen;
		char c;

		g_string_append(str, "\n\t");
		maxlen = (len < config_snaplen) ? len : config_snaplen;

		for (i=0;i<maxlen;i++)
			g_string_sprintfa(str, "%02x", frame_buf->head[i]);
		g_string_append(str, "\n\t");

		for (i=0;i<maxlen;i++) {
			c = frame_buf->head[i];
			if (c < 32 || c > 126)
				c='.';
			g_string_sprintfa(str, " %c", c);
		}
	}
	if (config_dump_bytes) {
		int i, maxlen;

		maxlen = (len < config_snaplen) ? len : config_snaplen;

		for (i=0;i<maxlen;i++) {
			if((i % config_snapcols) == 0)
				g_string_append(str, "\n\t");
			g_string_sprintfa(str, "%02x ", frame_buf->head[i]);
		}
	}
}

/*
 * Function irdadump_loop ()
 *
 *
 *
 */
int irdadump_loop(GString *str)
{
	struct sockaddr_ll from;
	int fromlen;
	int len;

	g_netbuf_recycle(frame_buf);

	/* Read packets from capture file */
	if (config_captureread >= 0) {
		int dir;
		int prot;
		int ret;

		ret = capread_get(config_captureread,
				  frame_buf,
				  &len,
				  &dir,
				  &prot,
				  curr_time);
		if(ret < 0) {
			perror("read capture file");
			exit(-1);
		}

		/* Filter away all non IrDA frames */
		if (prot != ETH_P_IRDA)
			return -1;

		/* Filter away empty frames (forced speed change) */
		if (len == 0)
			return -1;

		/* Data should be fine now */
		g_netbuf_put(frame_buf, len);

		/* Regular parsing */
		parse_irda_frame(dir, frame_buf, len, str);

		return(0);
	}

	/* Extract packet from socket */
	fromlen = sizeof(struct sockaddr_ll);

	len = recvfrom(fd, frame_buf->data, MAX_FRAME_SIZE, 0,
		       (struct sockaddr *) &from, &fromlen);
	if (len < 0) {
		g_message("recvfrom");
		exit(-1);
	}

	/* Filter away all non IrDA frames */
	if (from.sll_protocol != ntohs(ETH_P_IRDA))
		return -1;

	/* Filter away frames from other IrDA interfaces */
	if (ifindex && (from.sll_ifindex != ifindex))
		return -1;

	/* Filter away empty frames (forced speed change) */
	if (len == 0)
		return -1;

	/* Data should be fine now */
	g_netbuf_put(frame_buf, len);

	/* Get time from packet */
	if (ioctl(fd, SIOCGSTAMP, curr_time) < 0) {
		perror("ioctl");
		exit(-1);
	}

	/* Regular parsings */
	parse_irda_frame(from.sll_pkttype, frame_buf, len, str);

	/* Write packet into cature file if needed */
	if (config_capturewrite >= 0) {
		capwrite_dump(config_capturewrite,
			      frame_buf,
			      len,
			      &from,
			      curr_time);
	}

	return 0;
}
