/*********************************************************************
 *                
 * Filename:      discard.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 * Created at:    Mon May 10 10:08:04 1999
 * Modified at:   Mon May 10 10:10:23 1999
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

/*
 * Function irobex_listen (self)
 *
 *    Wait for incomming connections
 *
 */
gint irobex_listen(obex_t *self, char *service)
{
	int addrlen = sizeof(struct sockaddr_irda);
	int oflags;
	int mtu;
	int len = sizeof(int);

	DEBUG(3, __FUNCTION__ "()\n");

	/* Bind local service */
	self->trans.self.irda.sir_family = AF_IRDA;

	if (service == NULL)
		strncpy(self->trans.self.irda.sir_name, "OBEX", 25);
	else
		strncpy(self->trans.self.irda.sir_name, service, 25);

	self->trans.self.irda.sir_lsap_sel = LSAP_ANY;
	
	if (bind(self->fd, (struct sockaddr*) &self->trans.self.irda, 
		 sizeof(struct sockaddr_irda))) 
	{
	     perror("bind");
	     return -1;
	}

	if (listen(self->fd, 2)) {
		perror("listen");
		return -1;
	}
	self->fd = accept(self->fd, (struct sockaddr *) &self->trans.peer.irda,
 			  &addrlen);
	if (self->fd < 0) {
		perror("accept");
		return -1;
	}
	/* Check if we should register for SIGIO */
	if (self->async) {
		/* Register for asynchronous notification */
		signal(SIGIO, &input_handler);
		fcntl(self->fd, F_SETOWN, getpid());
		oflags = fcntl(0, F_GETFL);
		if( fcntl(self->fd, F_SETFL, oflags | FASYNC) < 0) {
			perror("fcntl");
			return -1;
		}
		
		async_self = self;
	}
	/* Check what the IrLAP data size is */
	if (getsockopt(self->fd, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, (void *) &mtu, 
		       &len)) 
	{
		perror("getsockopt");
		exit(-1);
	}
	self->trans.mtu = mtu;
	DEBUG(3, __FUNCTION__ "(), transport mtu=%d\n", mtu);

	return 0;
}

/*
 * Function irobex_discover_devices (fd)
 *
 *    Try to discover some remote device(s) that we can connect to
 *
 */
gint irobex_discover_devices(obex_t *self)
{
	struct irda_device_list *list;
	unsigned char *buf;
	int len;
	int i;

	len = sizeof(struct irda_device_list) +
		sizeof(struct irda_device_info) * MAX_DEVICES;

	buf = g_malloc(len);
	list = (struct irda_device_list *) buf;
	
	if (getsockopt(self->fd, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) {
		perror("getsockopt");
		exit(-1);
	}

	if (len > 0) {
		g_print("Discovered: (list len=%d)\n", list->len);

		for (i=0;i<list->len;i++) {
			g_print("  name:  %s\n", list->dev[i].info);
			g_print("  daddr: %08x\n", list->dev[i].daddr);
			g_print("  saddr: %08x\n", list->dev[i].saddr);
			g_print("\n");
			
			self->trans.peer.irda.sir_addr = list->dev[i].daddr;
			self->trans.self.irda.sir_addr = list->dev[i].saddr;
			
			/* Make sure we discovered an OBEX device */
			if (list->dev[i].hints[1] & HINT_OBEX) {
				DEBUG(0, __FUNCTION__ "(), this one looks good\n");
				return 0;
			}
		}
	}
	DEBUG(0, __FUNCTION__ "(), didn't find any OBEX devices!\n");
	return -1;
}

gint obex_data_indication(obex_t *self)
{
	obex_common_hdr_t *hdr;
	GNetBuf *msg;
	int final;
	int actual;
	guint8 opcode;
	int size;

	DEBUG(4, __FUNCTION__ "()\n");

	g_assert(self != NULL);

	msg = self->rx_msg;

	actual = recv(self->fd, msg->tail, g_netbuf_tailroom(msg), 0);

	DEBUG(4, __FUNCTION__ "(), got %d bytes\n", actual);
}
