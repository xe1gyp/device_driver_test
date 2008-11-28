/*********************************************************************
 *                
 * Filename:      client.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Mon Mar  8 14:32:49 1999
 * Modified at:   Wed Jan 19 18:06:05 2000
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999-2000 Dag Brattli, All Rights Reserved.
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
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <asm/byteorder.h>

#include <net/if_packet.h>

#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/types.h>
#include <irda.h>

#ifndef AF_IRDA
#define AF_IRDA 23
#endif

#define MAX_DEVICES 10

int discover_devices(int fd)
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
	printf("len=%d\n", len);
	if (len > 0) {
		printf("Discovered:\n");

		for (i=0;i<list->len;i++) {
			printf("  name:  %s\n", list->dev[i].info);
			printf("  daddr: %08x\n", list->dev[i].daddr);
			printf("  saddr: %08x\n", list->dev[i].saddr);
			printf("\n");
		}
	}
	 	    
	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	int count;
	unsigned char buf[2048];
	struct sockaddr_irda peer;
	int fromlen;
	
        /* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }
	printf("sizeof(struct irda_device_list)=%d\n", 
	       sizeof(struct irda_device_list));
	printf("sizeof(struct irda_device_info)=%d\n", 
	       sizeof(struct irda_device_info));

	discover_devices(fd);
#if 0	
	peer.sir_family = AF_IRDA;
	strncpy(peer.sir_name, "P1284", 25);

	if (connect(fd, (struct sockaddr *) &peer, 
		    sizeof(struct sockaddr_irda))) {
		perror("connect");
		exit(-1);
	}
#endif
	while(1)
		sleep(1);

	return 0;
}



