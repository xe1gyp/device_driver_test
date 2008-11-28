/*********************************************************************
 *                
 * Filename:      irdaspray.c
 * Version:       
 * Description:   
 * Status:        Experimental.
 * Author:        Dag Brattli <dagb@cs.uit.no>
 *		  Jean Tourrilhes <jt@hpl.hp.com>
 * Created at:    10/12/99
 * Modified at:   Mon May 10 18:31:35 1999
 * Modified by:   Dag Brattli <dagb@cs.uit.no>
 * 
 *     Copyright (c) 1999 Dag Brattli, All Rights Reserved.
 *     Copyright (c) 2001 Jean Tourrilhes, All Rights Reserved.
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
gcc -O2 -Wall -o irdaspray irdaspray.c
*/

/*
 * This program demonstrate the use of auto-connect (connect to a service
 * without having to discover/specify an address) and how a program
 * can report discovery results to the user in case the service is not
 * unique on the network.
 * Of course, you need both auto-connect and query-ias in the kernel, and
 * a few bugs sorted out...
 *
 * Then, we show a cleaner way to perform discovery, by having better
 * memory allocation strategy, better handling of errors and setting the
 * hint mask in the kernel...
 *
 * Also, we include a nice feature which is a blocking discovery. Basically,
 * we ask the kernel to wake us up when it discover a device. This reduces
 * CPU usage (to zero) and decrease latency (to almost zero). This is
 * geared toward mobile devices (or to serve those mobile devices), but
 * also to implement "irmanager/irmonitor" kind of functionality...
 *
 * Also, everything about manipulating the IAS database and making query
 * is explained. That allow you to play with non-standard fields.
 *
 * Note that each feature require specific kernel support in the IrDA stack
 * so please check if your IrDA stack support that... In fact, this was
 * mostly written to test those kernel features.
 *
 * Jean Tourrilhes
 */

/* CONFIGURATION */
#define WAITFORDEVICE	1
#define CHECKBEFOREWAIT	1
#define SETHINTMASK	1
#define AUTOCONNECT	1
#define GETSOCKNAME	1
#define SETIAS		0
#define DELIAS		0

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

int frame_size = 1024;
int frame_number = 100;
unsigned char buf[8200];
int echo = 1;		/* Use echo service by default (0 = discard) */
int server = 0;		/* Act as a client by default (1 = server) */
int two = 0;		/* Open only one client sockets (1 = two) */

/*
 * Function irdaspray_sethintmask (fd)
 *
 *    Set the hint mask in the kernel to filter discoveries
 *
 * Note : Unless explicitely changed, this setting will remain valid
 * for any subsequent discoveries on this socket, such as :
 *	o Waiting for device -> getsockopt(IRLMP_WAITDEVICE)
 *	o Auto-connect -> connect(daddr = 0x0)
 *	o Standard discovery -> getsockopt(IRLMP_ENUMDEVICES)
 */
void irdaspray_sethintmask(int fd)
{
	unsigned char	hints[4];

	/*
	 * The hint mask we use is set to catch any device that potentially
	 * support things outside IrComm/IrLAN/IrLPT, so most device
	 * worth connecting to via IrTTP.
	 * Setting the hint mask with IrComm or IrObex could be usefull
	 * for some apps...
	 */
	hints[0] = HINT_COMPUTER | HINT_PDA;
	hints[1] = 0;

	/* Set the filter used for performing discovery */
	if (setsockopt(fd, SOL_IRLMP, IRLMP_HINT_MASK_SET,
		       hints, sizeof(hints))) 
	{
		perror("setsockopt");
		exit(-1);
	}
}

/*
 * Function irdaspray_check_discovery (fd)
 *
 *    Check the state of the discovery log
 *
 * See irdaspray_discover_devices() for details
 */
int irdaspray_check_discovery(int fd)
{
	struct irda_device_list *	list;
	unsigned char		buf[DISC_BUF_LEN];
	int len;

	/* Set the list to point to the correct place */
	list = (struct irda_device_list *) buf;
	len = DISC_BUF_LEN;

	/* Perform a discovery and get device list */
	if (getsockopt(fd, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) {
		printf("Didn't find any devices!\n");
		return -1;
	}

	/* Did we got any ? (in some rare cases, this test is true) */
	if (list->len <= 0) {
		printf("Didn't find any devices!\n");
		return -1;
	}

	return 0;
}

/*
 * Function irdaspray_waitfor_devices (fd)
 *
 *    Wait until some device come into range...
 *
 * Note much to say. We set our hint mask, and call the stack with the
 * maximum amount of time we want to wait.
 * If a new device come in, we will wake up, otherwise it will timeout...
 * Note : this function is only triggered by *new* device, devices already
 * in range won't trigger this function.
 * The exact rules are :
 *	If a device has been discovered, or has change its hint bits,
 *	less than the discovery timeout in the past, then return,
 *	otherwise block until this happen or timeout.
 * Therefore, if you poll continuously this function, you'd better sleep
 * a bit after a positive result before calling it again...
 */
int irdaspray_waitfor_device(int fd)
{
	int timeout = 30000;	/* Block only for 30s */
	int len;

#if SETHINTMASK
	/* Set the filter used for performing discovery */
	irdaspray_sethintmask(fd);
#endif

#if CHECKBEFOREWAIT
	/* Note : before waiting for a new device to show up, we first
	 * check if a device has not already been discovered */
	if(irdaspray_check_discovery(fd) == 0)
		return 0;
#endif

	/* Wait for a device to be in range */
	printf("Waiting for a device...\n");
	len = sizeof(timeout);
	if (getsockopt(fd, SOL_IRLMP, IRLMP_WAITDEVICE, &timeout, &len)) {
		if(errno != EAGAIN) {
			perror("getsockopt-wait");
			exit(-1);
		}
		printf("Nothing found in the last %d s\n", timeout / 1000); 
		return -1;
	}

	return 0;	/* Found one device */
}

/*
 * Function echo_discover_devices (fd)
 *
 *    Try to discover some remote device(s) that we can connect to
 *
 * Note : in this function, the memory allocation for the discovery log
 * is done "the right way", so that we don't leak memory...
 */
int irdaspray_discover_devices(int fd, char *service_name)
{
	struct irda_device_list *	list;
	unsigned char		buf[DISC_BUF_LEN];
	struct irda_ias_set ias_query;
	int err;
	int len;
	int i;

	/* We are so proud of this feature that we show it again ! */
#if SETHINTMASK
	/* Set the filter used for performing discovery */
	irdaspray_sethintmask(fd);
#endif

	/* Set the list to point to the correct place */
	list = (struct irda_device_list *) buf;
	len = DISC_BUF_LEN;

	/* Perform a discovery and get device list */
	if (getsockopt(fd, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) {
		printf("Didn't find any devices!\n");
		return -1;
	}

	/* Did we got any ? (in some rare cases, this test is true) */
	if (list->len <= 0) {
		printf("Didn't find any devices!\n");
		return -1;
	}

	/* List all devices */
	printf("Discovered %d devices :\n", list->len);
	for (i=0;i<list->len;i++) {
		printf("  [%d] name:  %s, daddr: 0x%08x",
		       i + 1, list->dev[i].info, list->dev[i].daddr);
		fflush(stdout);

		/* Ask if the requested service exist on this device */
		len = sizeof(ias_query);
		ias_query.daddr = list->dev[i].daddr;
		strcpy(ias_query.irda_class_name, service_name);
		strcpy(ias_query.irda_attrib_name, "IrDA:TinyTP:LsapSel");
		err = getsockopt(fd, SOL_IRLMP, IRLMP_IAS_QUERY,
				 &ias_query, &len);
		if(err == 0) {
			printf(", has service %s\n", service_name);
		} else {
			if(errno != EADDRNOTAVAIL)
				printf(" <can't query IAS>\n");
			else
				printf(", doesn't have %s\n", service_name);
		}
	}
			
	/* Ask the user */
	printf("Enter device number:");
	fflush(stdout);
	if(scanf("%X", &i) != 1)
		return -1;
	i--;
	if((i < 0) && (i > list->len))
		return -1;
	return(list->dev[i].daddr);
}

/*
 * IRIAS set/get/query tests
 */
void irias_play(int	fd)
{
	struct irda_ias_set ias_query;
	int len;

	/* Get the name of our own device */
	len = sizeof(ias_query);
	strcpy(ias_query.irda_class_name, "Device");
	strcpy(ias_query.irda_attrib_name, "DeviceName");
	if (!getsockopt(fd, SOL_IRLMP, IRLMP_IAS_GET, &ias_query, &len)) {
		printf("The name of our device is = ``%s''\n",
		       ias_query.attribute.irda_attrib_string.string);
	}

	/* Get the name of the remote device */
	len = sizeof(ias_query);
	strcpy(ias_query.irda_class_name, "Device");
	strcpy(ias_query.irda_attrib_name, "DeviceName");
	if (!getsockopt(fd, SOL_IRLMP, IRLMP_IAS_QUERY, &ias_query, &len)) {
		printf("The name of the other device is = ``%s''\n",
		       ias_query.attribute.irda_attrib_string.string);
	}

	/* Set an integer in IAS (new object) */
	strcpy(ias_query.irda_class_name, "my:object");
	ias_query.irda_attrib_type = IAS_INTEGER;
	strcpy(ias_query.irda_attrib_name, "my:int");
	ias_query.attribute.irda_attrib_int = 45;
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_SET, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't set <my:object><my:int> in IAS\n");
	    perror("setsockopt");
	  }

	/* Set a string in IAS (existing object) */
	strcpy(ias_query.irda_class_name, "my:object");
	ias_query.irda_attrib_type = IAS_STRING;
	strcpy(ias_query.irda_attrib_name, "my:string");
	strcpy(ias_query.attribute.irda_attrib_string.string, "The String");
	ias_query.attribute.irda_attrib_string.len = strlen("The String");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_SET, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't set <my:object><my:string> in IAS\n");
	    perror("setsockopt");
	  }

	/* Set another string in IAS (system object) */
	strcpy(ias_query.irda_class_name, "Device");
	ias_query.irda_attrib_type = IAS_STRING;
	strcpy(ias_query.irda_attrib_name, "Owner");
	strcpy(ias_query.attribute.irda_attrib_string.string, "Myself");
	ias_query.attribute.irda_attrib_string.len = strlen("Myself");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_SET, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't set <Device><Owner> in IAS\n");
	    perror("setsockopt");
	  }

	/* Set yet another string in IAS (same object name as this socket) */
	if (echo)
		strcpy(ias_query.irda_class_name, "IrECHO");
	else
		strcpy(ias_query.irda_class_name, "IrDISCARD");
	ias_query.irda_attrib_type = IAS_STRING;
	strcpy(ias_query.irda_attrib_name, "App-Name");
	strcpy(ias_query.attribute.irda_attrib_string.string, "irdaspray.magic");
	ias_query.attribute.irda_attrib_string.len = strlen("irdaspray.magic");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_SET, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't set <IrECHO><App-Name> in IAS\n");
	    perror("setsockopt");
	  }

	/* Set another integer in IAS (the object attached to this socket) */
	strcpy(ias_query.irda_class_name, "");
	ias_query.irda_attrib_type = IAS_INTEGER;
	strcpy(ias_query.irda_attrib_name, "App-Version");
	ias_query.attribute.irda_attrib_int = 3;
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_SET, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't set <><App-Version> in IAS\n");
	    perror("setsockopt");
	  }

	/* Try to set a kernel attribute (should fail) */
	strcpy(ias_query.irda_class_name, "Device");
	strcpy(ias_query.irda_attrib_name, "DeviceName");
	strcpy(ias_query.attribute.irda_attrib_string.string, "Linux");
	ias_query.attribute.irda_attrib_string.len = strlen("Linux");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_SET, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't set <Device><DeviceName> in IAS\n");
	    perror("setsockopt");
	  }

	/* Read the integer in IAS */
	len = sizeof(ias_query);
	strcpy(ias_query.irda_class_name, "my:object");
	strcpy(ias_query.irda_attrib_name, "my:int");
	if (!getsockopt(fd, SOL_IRLMP, IRLMP_IAS_GET, &ias_query, 
		       &len)) 
	  {
	    printf("Type = %d\n", ias_query.irda_attrib_type);
	    printf("Int = %d\n", ias_query.attribute.irda_attrib_int);
	  }

	/* Read the string in IAS */
	len = sizeof(ias_query);
	strcpy(ias_query.irda_class_name, "my:object");
	strcpy(ias_query.irda_attrib_name, "my:string");
	if (!getsockopt(fd, SOL_IRLMP, IRLMP_IAS_GET, &ias_query, 
		       &len)) 
	  {
	    printf("Type = %d\n", ias_query.irda_attrib_type);
	    printf("String = ``%s''\n", ias_query.attribute.irda_attrib_string.string);
	  }
}

void irias_remove(int	fd)
{
	struct irda_ias_set ias_query;

	/* Delete the integer in IAS */
	strcpy(ias_query.irda_class_name, "my:object");
	strcpy(ias_query.irda_attrib_name, "my:int");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <my:object><my:int> in IAS\n");
	    perror("setsockopt");
	  }

	/* Delete the string in IAS */
	strcpy(ias_query.irda_class_name, "my:object");
	strcpy(ias_query.irda_attrib_name, "my:string");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <my:object><my:string> in IAS\n");
	    perror("setsockopt");
	  }

	/* Delete the other string in IAS */
	strcpy(ias_query.irda_class_name, "Device");
	strcpy(ias_query.irda_attrib_name, "Owner");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <Device><Owner> in IAS\n");
	    perror("setsockopt");
	  }

	/* Delete the last string in IAS */
	if (echo)
		strcpy(ias_query.irda_class_name, "IrECHO");
	else
		strcpy(ias_query.irda_class_name, "IrDISCARD");
	strcpy(ias_query.irda_attrib_name, "App-Name");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <IrECHO><App-Name> in IAS\n");
	  }

	/* Delete the other integer in IAS */
	strcpy(ias_query.irda_class_name, "");
	strcpy(ias_query.irda_attrib_name, "App-Version");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <><App-Version> in IAS\n");
	    perror("setsockopt");
	  }

	/* Try to delete a kernel attribute (should fail) */
	strcpy(ias_query.irda_class_name, "Device");
	strcpy(ias_query.irda_attrib_name, "DeviceName");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <Device><DeviceName> in IAS\n");
	    perror("setsockopt");
	  }

	/* Try to delete a non-existing attribute (should fail) */
	strcpy(ias_query.irda_class_name, "None");
	strcpy(ias_query.irda_attrib_name, "None");
	if (setsockopt(fd, SOL_IRLMP, IRLMP_IAS_DEL, &ias_query, 
		       sizeof(ias_query))) 
	  {
	    printf("Can't remove <None><None> in IAS\n");
	    perror("setsockopt");
	  }
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
	struct sockaddr_irda self;
	char *service_name;
	int len = sizeof(int);
	int daddr;
#if AUTOCONNECT
	int err;
#endif

	/* Set the service name */
	if (echo)
		service_name = "IrECHO";
	else
		service_name = "IrDISCARD";

#if WAITFORDEVICE
	/* Wait until a device is in range */
	if(irdaspray_waitfor_device(fd))
		return -1;
#endif

#if AUTOCONNECT
	/*
	 * First, we try the auto-connect, which in 99% of the case
	 * should be good enough...
	 *
	 * Auto connect lookup devices in range and query them about the
	 * service we want. If there is only one device that support
	 * this service, we are magically connected to it...
	 */
	peer.sir_family = AF_IRDA;
	strncpy(peer.sir_name, service_name, 25);
	peer.sir_addr = 0x0;	/* Maybe DEV_ADDR_ANY is better ? */
	
	err = connect(fd, (struct sockaddr*) &peer, 
		      sizeof(struct sockaddr_irda));

	/* Check what has happened */
	if(err == 0) {
		printf("Auto-connect did found exactly one device !\n");
		return 0;
	}
	if(errno == EADDRNOTAVAIL) {
		printf("Auto-connect could not find anything...\n");
		return -1;
	}
	if(errno != ENOTUNIQ)
		printf("Auto-connect failed...\n");
	else
		printf("Auto-connect has found more than one device...\n");
#endif

	/*
	 * At this point, if we don't have any user interface or if we
	 * don't want to bother with that, we could just tell the user
	 * to aim its device closer to the target and just quit...
	 * (or we could pretend that we have seen nothing, which will
	 * end up with the same result on the user)
	 * However, for the purpose of the exercise, let's pretend that
	 * the user doesn't want to move his device and has plenty of UI...
	 */

	/* Make a proper discovery, display device and ask user to choose */
	daddr = irdaspray_discover_devices(fd, service_name);
	if (daddr == -1)
		return -1;

	/* Now we can try again to connect using the address selected */
	peer.sir_family = AF_IRDA;
	strncpy(peer.sir_name, service_name, 25);
	peer.sir_addr = daddr;
	
	if (connect(fd, (struct sockaddr*) &peer, 
		    sizeof(struct sockaddr_irda))) {
		perror("connect");
		return -1;
	}

#if GETSOCKNAME
	/* Get out info about our socket (local, not remote) */
	len = sizeof(struct sockaddr_irda);
	if (getsockname(fd, (struct sockaddr*) &self, &len)) {
		perror("getsockname");
		return -1;
	}
	printf("We have been assigned Address 0x%X and LSAP 0x%02X...\n",
	       self.sir_addr, self.sir_lsap_sel);
	/* Note : getsockname return a valid lsap after a connect or a bind,
	 * but return a valid addr only after an accept, and with my patch
	 * also after a connect.
	 */
#endif

	printf("Connected!\n");

	/* Test if IR-IAS is happy */
#if SETIAS
	irias_play(fd);
#endif
#if DELIAS
	irias_remove(fd);
#endif

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

/*
 * Function ir_client ()
 *
 *    Implements IrDA Echo or Discard client
 *
 */
int ir_client(void)
{
	int fd;		/* The most important : the socket !!! */
	struct timeval start, end;
	int total;
	double time;
	int status;
	int ret;
	int pid = 0;		/* Sender/Receiver */
	int tpid = 0;		/* First/second instance */

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_SEQPACKET, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Try connect */
	ret = irdaspray_connect_request(fd);
	if (ret) {
		return -1;
	}
	
	gettimeofday(&start, (struct timezone*) 0);

	/* Check if we need to have two clients going on */
	if(two) {
		/* Spawn second instances */
		tpid = fork();
		/* If in parent */
		if(tpid) {
			/* Close socket */
			close(fd);
			/* Open a new socket */
			fd = socket(AF_IRDA, SOCK_SEQPACKET, 0);
			if (fd < 0) {
				perror("socket");
				exit(-1);
			}
			/* Connect the new socket */
			ret = irdaspray_connect_request(fd);
			if (ret) {
				return -1;
			}
		}
	}

	if (echo) {
		/* Fork off transmitter */
		pid = fork();
		
		if (pid) {	/* parent */
			total = irdaspray_receive(fd);
		} else {	/* child */
			total = irdaspray_transmit(fd);
		}
	} else			/* single */
		total = irdaspray_transmit(fd);

	/* pid==0: single-thread(discard-tx) or child(echo-tx)
	 * pid!=0: parent(echo-rx)
	 */

	gettimeofday(&end, (struct timezone*) 0);

	time = (double) (end.tv_sec - start.tv_sec) + (double)
		((double) (end.tv_usec - start.tv_usec) / 1000000.0);

	if (pid) {	/* parent */
		wait(&status);
		printf("Received %d bytes in %f seconds (%0.3f kbytes/s)\n",
		       total, time, (double) (total / time) / 1024); 
	}
	else {		/* child or single-thread */
		printf("Transmitted %d bytes in %f seconds (%0.3f kbytes/s)\n",
		       total, time, (double) (total / time) / 1024);
	}

	return 0;
}

/*
 * Function ir_server ()
 *
 *    Implements IrDA Echo or Discard server
 *
 */
int ir_server(void)
{
	struct sockaddr_irda peer, self;
	int addrlen;
	int actual;
	int fd, conn_fd;
	int pid = 0;

	/* Check personality */
	if (echo)
		printf("IrDA Echo server starting ...\n");
	else
		printf("IrDA Discard server starting ...\n");

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_SEQPACKET, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Init self */
	self.sir_family = AF_IRDA;
	if (echo)
		strncpy(self.sir_name, "IrECHO", 25);
	else
		strncpy(self.sir_name, "IrDISCARD", 25);

	self.sir_lsap_sel = LSAP_ANY;
	
	if (bind(fd, (struct sockaddr*) &self, sizeof(struct sockaddr_irda))) {
		perror("bind");
		return -1;
	}

	/* Test if IR-IAS is happy */
#if SETIAS
	irias_play(fd);
#endif
#if DELIAS
	irias_remove(fd);
#endif

	if (listen(fd, 8)) {
		perror("listen");
		return -1;
	}

	for (;;) {
		addrlen = sizeof(struct sockaddr_irda);

		printf("Waiting for connection!\n");

		/* collect old server zombies */
		while (waitpid(-1, NULL, WNOHANG) > 0)
			;

		conn_fd = accept(fd, (struct sockaddr *) &peer, &addrlen);
		if (conn_fd < 0) {
			perror("accept");
			return -1;
		}

		/* Fork off receiver, so that we can accept multiple
		 * simultaneous connections */
		pid = fork();

		/* If in children, receive, in parent, listen */
		if(!pid) {
			printf("Connected!\n");
		
			do {
				actual = recv(conn_fd, &buf, sizeof(buf), 0);
				if (actual <= 0) 
					break;

				printf("Got %d bytes\n", actual);
			
				if (echo) {
					actual = send(conn_fd, &buf, actual, 0);
					printf("Sent %d bytes\n", actual);
				}
			} while (actual > 0);

			close(conn_fd);
			printf("Disconnected!\n");

			/* Quit the child */
			return(0);
		}
	}
	return 0;
}

static void usage(char *argv[])
{
	fprintf(stderr, "usage: %s [-s] [-c] [-d] [-e] [-h] [-b frame-size] [-n frames] \n", argv[0]);
	fprintf(stderr, "      -s act as a server\n");
	fprintf(stderr, "      -c act as a client (default)\n");
	fprintf(stderr, "      -d use discard service for unidirection transfer\n");
	fprintf(stderr, "      -e use echo service for bidirection transfer (default)\n");
	fprintf(stderr, "      -t open two concurent client sockets\n");
	fprintf(stderr, "      -h print this message ;-)\n");
	fprintf(stderr, "      -b frame-size in bytes (default 1024, max 8192)\n");
	fprintf(stderr, "      -n number of frames (default 100)\n");
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
	int c;

	while ((c = getopt(argc, argv, "vscdethb:m:n:f:")) != -1) {
		switch (c) {
			
 		case 's':
			server = 1; /* Act as a server */
 			break;
			
 		case 'c':
			server = 0; /* Act as a client */
 			break;
			
 		case 'd':
 			echo = 0; /* Use discard service instead of echo */
 			break;
			
 		case 'e':
 			echo = 1; /* Use echo service instead of discard */
 			break;
			
 		case 't':
			two = 1; /* Open two client sockets */
 			break;
			
		case 'h':
			usage(argv);
			break;
			
		case 'n':
			frame_number = atoi(optarg);
			break;
			
		case 'b':
			frame_size = atoi(optarg);
			break;

		default:
			usage(argv);
			break;
		}
	}

	/* Start client or server as needed */
	if(server)
		return(ir_server());
	else
		return(ir_client());
}
