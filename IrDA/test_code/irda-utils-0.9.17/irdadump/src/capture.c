/*********************************************************************
 *                
 * Filename:      capture.c
 * Version:       0.1
 * Description:   Generate libpcap log files for Ethereal
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>		/* exit */
#include <unistd.h>		/* write */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "netbuf.h"
#include "capture.h"

#include <netpacket/packet.h>

#include <linux/types.h>

/*
 * Linux-IrDA capture type
 */
#define DLT_LINUX_IRDA		144

/*
 * SLL pseudo header definitions
 */

#define SLL_HDR_LEN	16		/* total header length */
#define SLL_ADDRLEN	8		/* length of address field */

struct sll_header {
	u_int16_t sll_pkttype;		/* packet type */
	u_int16_t sll_hatype;		/* link-layer address type */
	u_int16_t sll_halen;		/* link-layer address length */
	u_int8_t sll_addr[SLL_ADDRLEN];	/* link-layer address */
	u_int16_t sll_protocol;		/* protocol */
};

/* The LINUX_SLL_ values for "sll_pkttype" - see if_packet.h */
#define LINUX_SLL_HOST		0
#define LINUX_SLL_BROADCAST	1
#define LINUX_SLL_MULTICAST	2
#define LINUX_SLL_OTHERHOST	3
#define LINUX_SLL_OUTGOING	4

/* The LINUX_SLL_ values for "sll_protocol" - see if_ether.h */
#define LINUX_SLL_P_IRDA	0x0017	/* IrLAP frames */


#define SENT_FRAME  0x0100
#define RECV_FRAME  0x0101

struct pcaprec_hdr {
	__u32 ts_sec;
	__u32 ts_usec;
	__u32 incl_len;
	__u32 orig_len;
};

#if 0
struct pseudo_hdr {
	__u8  not_used1;
	__u8  not_used2;
	__u16 type;
};
#endif

struct pcap_hdr {
	__u32 magic;
	__u16 version_major;
	__u16 version_minor;
	__u32 thiszone;
	__u32 sigfigs;
	__u32 snaplen;
	__u32 network;
};

struct pcap_hdr fileHeader = {
	magic:		0xa1b2c3d4,
	version_major:	2,
	version_minor:	0,
	thiszone:	0,
	sigfigs:	0,
	snaplen:	2050 + sizeof(struct sll_header),
	network:	DLT_LINUX_IRDA
};


/*
 * Function capwrite_open ()
 *
 *    Open capture file and return its fd
 *
 */
int capwrite_open(char *	capfilename)
{
	int capturefile;

	/* Open the capture file */
	capturefile = open(capfilename, O_WRONLY | O_CREAT | O_TRUNC, 00640);
	if (capturefile < 0)
		perror("Opening capture file");

	return capturefile;
}

/*
 * Function capwrite_close ()
 *
 *    Close capture file if needed
 *
 */
void capwrite_close(int capturefile)
{
	close(capturefile);
}

/*
 * Function capwrite_init ()
 *
 *    Write capture header in file
 *
 */
int capwrite_init(int capturefile)
{
	/* Write capture file header */
	if (write(capturefile, &fileHeader, sizeof(struct pcap_hdr)) < 0)
		return -1;

	return 0;
}

/*
 * Map the standard cooked Linux header to the capture format.
 * Cut'n'pasted from libpcap (BSD license).
 * Jean II
 */
int capwrite_fillsll(struct sll_header	*hdrp,
		     struct sockaddr_ll *from)
{
	switch (from->sll_pkttype) {

	case PACKET_HOST:
		hdrp->sll_pkttype = htons(LINUX_SLL_HOST);
		break;

	case PACKET_BROADCAST:
		hdrp->sll_pkttype = htons(LINUX_SLL_BROADCAST);
		break;

	case PACKET_MULTICAST:
		hdrp->sll_pkttype = htons(LINUX_SLL_MULTICAST);
		break;

	case PACKET_OTHERHOST:
		hdrp->sll_pkttype = htons(LINUX_SLL_OTHERHOST);
		break;

	case PACKET_OUTGOING:
		hdrp->sll_pkttype = htons(LINUX_SLL_OUTGOING);
		break;

	default:
		hdrp->sll_pkttype = -1;
		break;
	}

	hdrp->sll_hatype = htons(from->sll_hatype);
	hdrp->sll_halen = htons(from->sll_halen);
	memcpy(hdrp->sll_addr, from->sll_addr,
	       (from->sll_halen > SLL_ADDRLEN) ?
	       SLL_ADDRLEN :
	       from->sll_halen);
	hdrp->sll_protocol = from->sll_protocol;
}

/*
 * Function capwrite_dump ()
 *
 *    Write capture packet in file
 *
 */
int capwrite_dump(int capturefile,
		 GNetBuf *frame_buf,
		 int len,
		 struct sockaddr_ll *from,
		 struct timeval *curr_time)
{
	struct pcaprec_hdr rec_hdr;
	struct sll_header  psd_hdr;

	rec_hdr.ts_sec    = curr_time->tv_sec;
	rec_hdr.ts_usec   = curr_time->tv_usec;
	rec_hdr.orig_len  = len + sizeof(psd_hdr);
	rec_hdr.incl_len  = rec_hdr.orig_len;

	capwrite_fillsll(&psd_hdr, from);

	if ((write(capturefile, &rec_hdr,
			sizeof(rec_hdr)) < 0) ||
		(write(capturefile, &psd_hdr,
			sizeof(psd_hdr)) < 0) ||
		(write(capturefile, frame_buf->head, len) < 0)) {
		perror("write capture file");
		exit(-1);
	}

	return 0;
}

/*
 * Function capread_open ()
 *
 *    Open capture file and return its fd
 *
 */
int capread_open(char *	capfilename)
{
	int capturefile;

	/* Open the capture file */
	capturefile = open(capfilename, O_RDONLY, 00640);
	if (capturefile < 0)
		perror("Opening capture file");

	return capturefile;
}

/*
 * Function capread_close ()
 *
 *    Close capture file if needed
 *
 */
void capread_close(int capturefile)
{
	close(capturefile);
}

/*
 * Function capread_check ()
 *
 *    Read capture header from file and verifies it
 *
 */
int capread_check(int capturefile)
{
	struct pcap_hdr readHeader;

	/* Write capture file header */
	if (read(capturefile, &readHeader, sizeof(struct pcap_hdr))
	    != sizeof(struct pcap_hdr))
		return -1;

	/* Compare */
	if((readHeader.magic != fileHeader.magic) ||
	   (readHeader.version_major != fileHeader.version_major) ||
	   (readHeader.version_minor != fileHeader.version_minor) ||
	   (readHeader.network != fileHeader.network))
		return -1;

	return 0;
}

/*
 * Function capwrite_get ()
 *
 *    Read capture packet from file
 *
 */
int capread_get(int capturefile,
		GNetBuf *frame_buf,
		int *plen,
		int *pdir,
		int *pprot,
		struct timeval *curr_time)
{
	struct pcaprec_hdr rec_hdr;
	struct sll_header  psd_hdr;

	if (read(capturefile, &rec_hdr, sizeof(rec_hdr)) != sizeof(rec_hdr))
		return -1;

	curr_time->tv_sec = rec_hdr.ts_sec;
	curr_time->tv_usec = rec_hdr.ts_usec;
	*plen = rec_hdr.orig_len - sizeof(psd_hdr);

	if (read(capturefile, &psd_hdr, sizeof(psd_hdr)) != sizeof(psd_hdr))
		return -1;

	/* Meta-data */
	*pdir = psd_hdr.sll_pkttype != LINUX_SLL_HOST;
	*pprot = psd_hdr.sll_protocol;

	if (read(capturefile, frame_buf->head, *plen) != *plen)
		return -1;

	return 0;
}
