/*********************************************************************
 *
 * Filename:      main.c
 * Version:
 * Description:
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Sun Mar 28 12:28:51 1999
 * Modified at:   Wed Jan 19 11:05:52 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 *
 *     Copyright (c) 1999-2000 Dag Brattli, All Rights Reserved.
 *     Copyright (c) 2002-2003 Jean Tourrilhes, All Rights Reserved.
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

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>	/* strdup */
#include <fcntl.h>

#include <glib.h>

#include <irdadump.h>
#include <capture.h>

#define VERSION "0.9.17 (21.11.2003) Dag Brattli/Jean Tourrilhes"

int packets = 0;

void cleanup(int signo)
{

	/* Kill "unused" warning */
	signo = signo;

	fflush(stdout);
	putc('\n', stdout);

	printf("%d packets received by filter\n", packets);

	if (config_capturewrite >= 0)
		capwrite_close(config_capturewrite);
	if (config_captureread >= 0)
		capread_close(config_captureread);

	exit(0);
}

int main(int argc, char *argv[])
{
	GString *line;
	char *ifdev = NULL;
	char *writefilename = NULL;
	char *readfilename = NULL;
	int fd, c;

	while ((c = getopt(argc, argv, "bc:di:lpr:s:tvw:x?")) != -1) {
		switch (c) {
		case 'b': /* Dumb bytes */
			config_dump_bytes = 1;
			break;
		case 'c': /* set snapcols for byte printing */
 			c = atoi(optarg);
 			if (c <= 0) {
				config_snapcols = 16;
			} else {
 				config_snapcols = c ;
 			}
			break;
		case 'd': /* Print diffs */
			config_print_diff = 1;
			break;
		case 'i': /* Interface */
			ifdev = (char *) strdup(optarg);
			printf("Using interface: %s\n", ifdev);
			break;
 		case 'l': /* Set linebuffering */
 			setlinebuf(stdout);
 			break;
		case 'p': /* Disable IrDA frame parsing, in case they
			   * are garbage... Combine with -b or -x to
			   * get only bytes display. */
			config_print_irlap = 0;
			break;
		case 'r': /* Read libpcap log file - JeanII */
			readfilename = (char *) strdup(optarg);
			break;
		case 's': /* set snaplen for printing */
 			c = atoi(optarg);
 			if (c <= 0) {
				config_snaplen = 2050;
			} else {
 				config_snaplen = c ;
 			}
		case 't': /* Force TTP decoding of unknown connections */
			config_force_ttp = 1;
			break;
 		case 'v': /* version */
			printf("Version: %s\n", VERSION);
			exit(0);
		case 'w': /* Write libpcap log file - J.K. */
			writefilename = (char *) strdup(optarg);
			break;
		case 'x': /* Dump frame (byte + ascii) */
			config_dump_frame = 1;
			break;
 		case '?': /* usage */
			fprintf(stderr,"Usage: %s [-d] [-x] [-b] [-s <n>] [-c <n>] [-p] [-i device] [-o log]\n", 
				argv[0]);
 			fprintf(stderr,"\t-d\tPrint diffs\n");
 			fprintf(stderr,"\t-l\tSet line buffering on output file.\n");
			fprintf(stderr,"\t-s <n>\tSet snaplen for -x & -b\n");
 			fprintf(stderr,"\t-x\tDump frame (bytes + ascii)\n");
 			fprintf(stderr,"\t-b\tDump bytes in columns\n");
			fprintf(stderr,"\t-c <n>\tSet number of colums for -b\n");
			fprintf(stderr,"\t-p <n>\tDisable parsing/decoding\n");
			fprintf(stderr,"\t-i device\tIrDA port to listen on\n");
			fprintf(stderr,"\t-w log\tWrites out raw capture file\n");
			fprintf(stderr,"\t-r log\tRead from raw capture file\n");
 			exit(1);
		default:
			break;
		}
	}
	signal(SIGTERM, cleanup);
	signal(SIGINT, cleanup);
	signal(SIGHUP, cleanup);

	/* Open the capture file for writing. */
	if(writefilename != NULL) {
		config_capturewrite = capwrite_open(writefilename);
		if(config_capturewrite < 0)
			return config_capturewrite;
		if(capwrite_init(config_capturewrite) < 0)
			return -1;
	}
	/* Open the capture file for reading. */
	if(readfilename != NULL) {
		config_captureread = capread_open(readfilename);
		if(config_captureread < 0) {
			return config_captureread;
		}
		if(capread_check(config_captureread) < 0) {
			return -1;
		}
	}

	/* Initialise socket to IrDA stack */
	fd = irdadump_init(ifdev);
	if (fd < 0) {
	    perror(argv[0]);
	    return fd;
	}
	line = g_string_sized_new(1024);

	while (1) {
		/* Get a packet from the IrDA device */
		if (irdadump_loop(line) == -1)
			continue;

		packets++;

		/* Print line on the screen */
		puts(line->str);

		/* Recycle line */
		g_string_truncate(line, 0);
	}
	return 0;
}
