/*********************************************************************
 *                
 * Filename:      irprintf.c
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
 * Read an Ir socket and display it on stdout
 * Use stream
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

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

unsigned char buf[4098];

/*
 * Function main (argc, )
 *
 *    Implements IrDA Echo or Discard server
 *
 */
int main(int argc, char *argv[])
{
	struct sockaddr_irda peer, self;
	int addrlen;
	int fd, conn_fd;
	FILE *stream;

	printf("IrDA printf server starting ...\n");

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Init self */
	self.sir_family = AF_IRDA;
	strncpy(self.sir_name, "MyServer", 25);

	self.sir_lsap_sel = LSAP_ANY;
	
	if (bind(fd, (struct sockaddr*) &self, sizeof(struct sockaddr_irda))) {
		perror("bind");
		return -1;
	}

	if (listen(fd, 8)) {
		perror("listen");
		return -1;
	}

	for (;;) {
		addrlen = sizeof(struct sockaddr_irda);

		printf("Waiting for connection!\n");
		conn_fd = accept(fd, (struct sockaddr *) &peer, &addrlen);
		if (conn_fd < 0) {
			perror("accept");
			return -1;
		}
		stream = fdopen(conn_fd, "r");
		if(stream == NULL) {
			perror("fdopen");
			return -1;
		}
		printf("Connected!\n");
		
		do {
			if((fgets(buf, sizeof(buf), stream) == NULL) ||
			   (buf[0] == 0x3))
				buf[0] = '\0';

			fwrite(buf, 1, strlen(buf), stdout);

		} while (buf[0] != '\0');
		fflush(stdout);
		fclose(stream);
		close(conn_fd);
		printf("Disconnected!\n");
	}
	return 0;
}
