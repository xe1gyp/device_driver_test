/*********************************************************************
 *                
 * Filename:      echo.c
 * Version:       0.9
 * Description:   IrDA echo/discard service
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Mon May 10 10:08:04 1999
 * Modified at:   Mon May 10 12:08:44 1999
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

int discard = 0;         /* Default is the echo service */
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
	int actual;
	int fd, conn_fd;

	/* Check personality */
	if (strcmp(argv[0], "discard") == 0) {
		printf("IrDA Discard server starting ...\n");
		discard = 1;
	} else
		printf("IrDA Echo server starting ...\n");

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Init self */
	self.sir_family = AF_IRDA;
	if (discard)
		strncpy(self.sir_name, "IrDISCARD", 25);
	else
		strncpy(self.sir_name, "IrECHO", 25);

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
		printf("Connected!\n");
		
		do {
			actual = recv(conn_fd, &buf, sizeof(buf), 0);
			if (actual <= 0) 
				break;

			printf("Got %d bytes\n", actual);
			
			if (!discard) {
				actual = send(conn_fd, &buf, actual, 0);
				printf("Sent %d bytes\n", actual);
			}
		} while (actual > 0);

		close(conn_fd);
		printf("Disconnected!\n");
	}
	return 0;
}















