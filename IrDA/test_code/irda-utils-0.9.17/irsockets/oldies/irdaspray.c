/*********************************************************************
 *                
 * Filename:      irdaspray.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Mon May 10 10:12:55 1999
 * Modified at:   Wed Jan 19 18:06:30 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999-2000 Dag Brattli, All Rights Reserved.
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
#include <sys/wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <irda.h>

#ifndef AF_IRDA
#define AF_IRDA 23
#endif /* AF_IRDA */

#define MAX_DEVICES 10

int fd;
int mtu = 0;
int frame_size = 1024;
int frame_number = 100;
unsigned char buf[4096];
int delay = 0;
int echo = 0;             /* Use discard service by default */

/*
 * Function echo_discover_devices (fd)
 *
 *    Try to discover some remote device(s) that we can connect to
 *
 */
int irdaspray_discover_devices(int fd)
{
	struct irda_device_list *list;
	unsigned char *buf;
	int len;
	int i;

	len = sizeof(struct irda_device_list) +
		sizeof(struct irda_device_info) * MAX_DEVICES;

	buf = malloc(len);
	list = (struct irda_device_list *) buf;
	
	if (getsockopt(fd, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) {
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
			
			if (list->dev[i].hints[0] & HINT_COMPUTER) {
				printf("Lets try this one\n");
				return list->dev[i].daddr;
			}
		}
	}
	printf("Didn't find any devices!\n");

	return -1;
}

/*
 * Function irdaspray_connect_request (self)
 *
 *    Try to connect to remote device
 *
 */
static int irdaspray_connect_request(int fd)
{
	struct sockaddr_irda peer;
	int len = sizeof(int);
	int daddr;

	/*
	 *  Open TTP connection to remote device!
	 */
	daddr = irdaspray_discover_devices(fd);
	if (daddr == -1)
		return -1;

	/* Disable IrTTP SAR */
	if (setsockopt(fd, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, &mtu, 
		       sizeof(mtu))) 
	{
		perror("setsockopt");
		exit(-1);
	}

	peer.sir_family = AF_IRDA;
	if (echo)
		strncpy(peer.sir_name, "IrECHO", 25);
	else
		strncpy(peer.sir_name, "IrDISCARD", 25);
	peer.sir_addr = daddr;
	
	if (connect(fd, (struct sockaddr*) &peer, 
		    sizeof(struct sockaddr_irda))) {
		perror("connect");
		return -1;
	}

	/* Check what the IrLAP data size is */
	if (getsockopt(fd, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, 
		       (void *) &mtu, &len)) 
	{
		perror("getsockopt");
		exit(-1);
	}

	/* printf(__FUNCTION__ "(), link mtu=%d\n", mtu); */

	return 0;
}

int irdaspray_transmit(int fd)
{
	int total = 0;
	int actual;
	int i;

	/* Transmit frames */
	for (i=0; i<frame_number; i++) {
		actual = send(fd, buf, frame_size, 0);
		total += actual;
	}
	return total;
}

int irdaspray_receive(int fd)
{
	int total = 0;
	int actual;
	int i;

	/* Receive frames */
	for (i=0; i<frame_number; i++) {
		actual = recv(fd, buf, sizeof(buf), 0);
		total += actual;
	}
	return total;
}

static void usage(char *argv[])
{
	fprintf(stderr, "usage: %s [-v] [-e] [-h] [-b frame-size] [-n frames] [-f file] [device]\n", argv[0]);
	fprintf(stderr, "      -v verbose\n");
	fprintf(stderr, "      -e use echo service for full duplex transfer\n");
	fprintf(stderr, "      -h print this message\n");
	fprintf(stderr, "      -s frame-size in bytes (default 1024)\n");
	fprintf(stderr, "      -n number of frames (default 100)\n");
	fprintf(stderr, "      -f file to preload buffer (zero buffer by default)\n");
	fprintf(stderr, "      -d inter-buffer transmission delay in usecs (default 0)\n");
	exit(1);
}

/*
 * Function main (argc, )
 *
 *    
 *
 */
int main(int argc, char *argv[])
{
	struct timeval start, end;
	int total;
	double time;
	int status;
	int ret;
	int c;
	int pid = 0;

	while ((c = getopt(argc, argv, "vehs:m:n:f:d:")) != -1) {
		switch (c) {
 		case 'm':
 			mtu = atoi(optarg);
 			break;
			
 		case 'e':
 			echo = 1; /* Use echo service instead of discard */
 			break;
			
		case 'h':
			usage(argv);
			break;
			
		case 'n':
			frame_number = atoi(optarg);
			break;
			
		case 's':
			frame_size = atoi(optarg);
			break;
			
/* 		case 'f': */
/* 			fflag = TRUE; */
/* 			if ((infile = fopen(optarg, "r")) == NULL) { */
/* 				fprintf(stderr, "%s: Can't open file %s.\n", argv[0], optarg); */
/* 				exit(2); */
/* 			} */
/* 			break; */
			
		case 'd':
			delay = atoi(optarg);
			break;
			
		default:
			usage(argv);
			break;
		}
	}

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Try connect */
	ret = irdaspray_connect_request(fd);
	if (ret) {
		return -1;
	}
	
	printf("Connected!\n");

	gettimeofday(&start, (struct timezone*) 0);

	if (echo) {
		/* Fork off receiver */
		pid = fork();
		
		if (pid) {
			total = irdaspray_receive(fd);
		} else {
			total = irdaspray_transmit(fd);
		}
	} else
		total = irdaspray_transmit(fd);

	gettimeofday(&end, (struct timezone*) 0);

	time = (double) (end.tv_sec - start.tv_sec) + (double)
		((double) (end.tv_usec - start.tv_usec) / 1000000.0);

	if (pid) 
		printf("Received %d bytes in %f seconds (%0.3f kbytes/s)\n",
		       total, time, (double) (total / time) / 1024); 
	else {
		if (echo)
			wait(&status);
		
		printf("Transmitted %d bytes in %f seconds (%0.3f kbytes/s)\n",
		       total, time, (double) (total / time) / 1024);
	}

	return 0;
}
