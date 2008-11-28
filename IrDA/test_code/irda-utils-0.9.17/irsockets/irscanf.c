/*********************************************************************
 *                
 * Filename:      irscanf.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Authors:       Dag Brattli <dagb@cs.uit.no>
 *		  Jean Tourrilhes <jt@hpl.hp.com>
 * Created at:    7/12/99
 * Modified at:   
 * Modified by:   
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

/*
 * Read stdin and push it on an Ir socket
 * Use stream
 */

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

#include <linux/types.h>
#include <linux/irda.h>

#ifndef AF_IRDA
#define AF_IRDA 23
#endif /* AF_IRDA */

/* Memory allocation for discovery */
#define DISC_MAX_DEVICES 10
#define DISC_BUF_LEN	sizeof(struct irda_device_list) + \
			sizeof(struct irda_device_info) * DISC_MAX_DEVICES

unsigned char buf[4096];

/*
 * Function echo_discover_devices (fd)
 *
 *    Try to discover some remote device(s) that we can connect to
 *
 */
int irscanf_discover_devices(int fd)
{
	struct irda_device_list *list;
	unsigned char		buf[DISC_BUF_LEN];
	int len;
	int i;

	/* Set the list to point to the correct place */
	list = (struct irda_device_list *) buf;
	len = DISC_BUF_LEN;

	if((getsockopt(fd, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) ||
	   (list->len <= 0)) {
		perror("getsockopt");
		printf("Didn't find any devices!\n");
		return(-1);
	}

	/* List all devices */
	printf("Discovered %d devices :\n", list->len);
	for (i=0;i<list->len;i++) {
		printf("  [%d] name:  %s, daddr: 0x%08x\n",
		       i + 1, list->dev[i].info, list->dev[i].daddr);

	}

	/* Clever heuristic... */
	printf("Lets try this one\n");
	return list->dev[0].daddr;
}

/*
 * Function main (argc, )
 *
 *    
 *
 */
int main(int argc, char *argv[])
{
	struct sockaddr_irda peer;
	int daddr = 0;
	int fd;
	FILE *stream;
	int hints;

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Set the filter used for performing discovery */
	hints = HINT_COMPUTER | HINT_PDA;
	if (setsockopt(fd, SOL_IRLMP, IRLMP_HINT_MASK_SET, &hints, 
		       sizeof(hints))) 
	{
		perror("setsockopt-hints");
		exit(-1);
	}

#if 0
	/* Find a peer */
	daddr = irscanf_discover_devices(fd);
	if (daddr == -1)
		return -1;
#endif

	peer.sir_family = AF_IRDA;
	strncpy(peer.sir_name, "MyServer", 25);
	peer.sir_addr = daddr;
	
	if (connect(fd, (struct sockaddr*) &peer, 
		    sizeof(struct sockaddr_irda))) {
		perror("connect");
		return -1;
	}

	stream = fdopen(fd, "w");
	if(stream == NULL) {
		perror("fdopen");
		return -1;
	}

	printf("Connected!\n");
	printf("Type your text with '.' on the last line...\n");

	do {
		if((fgets(buf, sizeof(buf), stdin) == NULL) ||
		   (buf[0] == '.')) {
			buf[0] = 0x3;
			buf[1] = '\0';
		}

		fwrite(buf, 1, strlen(buf), stream);
		fflush(stream);
	} while(buf[0] != 0x3);

	printf("Closing connection...\n");
	fclose(stream);
	close (fd);

	return 0;
}
