/*********************************************************************
 *                
 * Filename:      psion5.c
 * Version:       0.3
 * Description:   User space application for Psion 5 Palmtop Computers
 * Status:        Experimental.
 * Author:        Fons Botman <budely@tref.nl>
 * Created at:    Mon Apr 19 21:51:29 CEST 1999
 * 
 *     Copyright (c) 1999, Fons Botman, All Rights Reserved.
 *      
 *     This program is free software; you can redistribute it and/or 
 *     modify it under the terms of the GNU General Public License as 
 *     published by the Free Software Foundation; either version 2 of 
 *     the License, or (at your option) any later version.
 *  
 *     Neither Fons Botman nor anyone else admit liability nor
 *     provide warranty for any of this software. This material is 
 *     provided "AS-IS" and at no charge.
 *     
 * Version history:
 *     0.3 Fons Botman, Fri Oct  1 22:11:11 CEST 1999
 *		Added some switches, workaround for discovery problem
 *     0.2 Fons Botman, Mon Sep 27 21:59:20 CEST 1999
 *		Fixed SDU size problem.
 *     0.1 Initial version.     
 *     
 ********************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <utime.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <irda.h>

#ifndef AF_IRDA
#define AF_IRDA 23
#endif /* AF_IRDA */

/* Default slot_timeout */
static int slot_timeout = 120;

static int debug_level = 0;
#define DEBUG(n, str...) ( debug_level >= (n) ? printf(str) : 0 )

static int verbose = 1;

#define MAX_DEVICES 10

int set_slot_timeout(int ms) {
	FILE* fp;
	char* varpath = "/proc/sys/net/irda/slot_timeout";
	fp = fopen(varpath,"w");
	if (fp == NULL) {
		perror(varpath);
		return 1;
	} else {
		DEBUG(1, "Setting slot_timeout to %d\n", slot_timeout);
		if (fprintf(fp,"%d\n", ms) <= 0) {
			perror(varpath);
		}
		if (fclose(fp) != 0) {
			perror(varpath);
		}
	}
	return 0;
}

int discover_devices(int fd)
{
	struct irda_device_list *list;
	unsigned char *buf;
	int len;
	int i;

	/* Newer include files have an irda_device_info array of size
	   one, so subtract 1 from MAX_DEVICES to be safe */
	len = sizeof(struct irda_device_list) +
		sizeof(struct irda_device_info) * (MAX_DEVICES-1);

	if (slot_timeout > 0)
		set_slot_timeout(slot_timeout);
	
	if (!(buf = malloc(len))) {
		fprintf(stderr, "Could not allocate discovery buffer.\n");
		exit(1);
	}
	list = (struct irda_device_list *) buf;

	for ( i=0 ; i<5 ; i++ ) {
		if (0 == getsockopt(fd, SOL_IRLMP, IRLMP_ENUMDEVICES, buf, &len)) {
			DEBUG(1, "getsockopt IRLMP_ENUMDEVICES ok, len=%d\n", 
				len);
			if (i == 0 && len == 4) {
				/* bug in 2.2.12 */
				DEBUG(1, "Ignoring first empty reply\n");
				sleep(1);
				continue;
			}
			break;
		}
		if (errno == EAGAIN) {
			/* retry after waiting a bit */
			DEBUG(1, "Waiting for discovery to finish.\n");
			sleep(1);
		} else {
			perror("getsockopt");
			exit(-1);
		}
	}

	if (len < 4) {
		fprintf(stderr,"Unexpected len value: %d\n", len);
		exit(1);
	}
	if (list->len > 0) {
		DEBUG(1, "Discovered: (%d,%d)\n", len, list->len);
		for (i=0;i<list->len;i++) {
			DEBUG(1,"  daddr: %08x  saddr: %08x  "
				"hints: %02x%02x  name: %s\n",
				list->dev[i].daddr, list->dev[i].saddr,
				list->dev[i].hints[0], list->dev[i].hints[1],
				list->dev[i].info);
		}
	} else {
		DEBUG(1, "No devices discovered.\n");
		return 0;
	}

	/* Select obvious candidates */
	for (i=0;i<list->len;i++) {
		if (0 == strncmp(list->dev[i].info,"Symbian EPOC",12)) {
			DEBUG(1, "Selecting first EPOC device.\n");
			if (verbose) printf("Sending to %s\n", list->dev[i].info);
			return list->dev[i].daddr;
		}
	}
	for (i=0;i<list->len;i++) {
		if (0 == strncmp(list->dev[i].info,"Psion Series 5",14)) {
			DEBUG(1, "Selecting first Psion Series 5 device.\n");
			if (verbose) printf("Sending to %s\n", list->dev[i].info);
			return list->dev[i].daddr;
		}
	}
	DEBUG(1, "Selecting first IRDA device.\n");
	if (verbose) printf("Sending to %s\n", list->dev[0].info);
	return list->dev[0].daddr;
}


int irttp_get_mtu(int fd) {
	int mtu;
	int len = sizeof(int);
	/* Check what the IrTTP data size is */
	if (getsockopt(fd, SOL_IRLMP, IRTTP_MAX_SDU_SIZE, 
		       (void *)&mtu, &len)) {
		perror("getsockopt IRTTP_MAX_SDU_SIZE");
		return -1;
	}
	DEBUG(1, "max_sdu_size = %d\n", mtu);
	return mtu;
}


int sendfile(char* filename) {
	int fd;
	struct sockaddr_irda peer;
	int daddr;
	FILE* f;
	int buflen;
	char *buf;
	int rc;
	struct stat s;
	int cnt;
	int t0, tx, t;
	unsigned long long int fdatell;

	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		if (errno == EINVAL) 
			fprintf(stderr, "Is IrDA active?, perhaps run irmanager\n");
		exit(-1);
	}

	/* FIXME: We should use a better/any device selection mechanism */
	daddr = discover_devices(fd);
	if (!daddr) {
		fprintf(stderr,"No IRDA device found\n");
		exit(1);
	}

	peer.sir_family = AF_IRDA;
	peer.sir_addr = daddr;
	strcpy(peer.sir_name, "Epoc32:EikonIr:v1.0");

	if (connect(fd, (struct sockaddr *) &peer, 
		    sizeof(struct sockaddr_irda))) {
		perror("connect");
		if (errno == ENETUNREACH)
			/* P5: System ^L IrDA is active, 
			   but IR-receive not selected */
			fprintf(stderr, 
				"No Psion5 or IR-receive is not selected\n");
			/* Maybe we should retry in this case */
		exit(-1);
	}
	DEBUG(1, "Connected to %x\n", daddr);

	buflen = irttp_get_mtu(fd);
	/* In case of linux compression, buflen may be 2044 */
	if (buflen != 2045 && buflen != 2044) {
		fprintf(stderr, "Expecting mtu to be 2045, investigate!\n");
	}
	DEBUG(1, "Setting mtu to 2044\n");
	buflen = 2044;
	if (!(buf = malloc(buflen))) {
		perror("malloc");
		exit(1);
	}
	/* 
	   FIXME : I got strange results when the buffer size was less than 
	   the mtu (e.g. 200), the psion did not seem to see the frames were 
	   not full length, and stopped after the number of frames based on 
	   the full mtu size.
	   investigate.
	*/

	/* 
	   FIXME : psion connects to port 2, but does not get error back
	   from us. Linux bug?
	*/

	if (!(f = fopen(filename,"rb"))) {
		perror(filename);
		exit(1);
	}

	rc = stat(filename,&s);
	if (rc != 0) {
		perror("stat");
		exit(1);
	}

	/* FIXME map psion mode bits to unix filemodes */

	fdatell = 62168263200000000ULL + 1000000 * 
		( s.st_mtime & 0x00000000FFFFFFFFULL);
	DEBUG(1, "date: %Ld\n", (unsigned long long) s.st_mtime);
	DEBUG(1, "date: %Ld\n", fdatell);
	sprintf(buf,"FILE %d %d %lu %lu %s", 
		(int) s.st_size,
		32 | (s.st_mode & S_IWUSR ? 0 : 1),
		(unsigned long) ((unsigned long long int) fdatell >> 32),
		(unsigned long) (fdatell & 0x00000000FFFFFFFFULL),
		filename);
	rc = write(fd,buf,strlen(buf));
	if (rc != strlen(buf)) {
		perror("write");
		fprintf(stderr,"rc = %d, strlen=%d\n", 
			rc, strlen(buf));
		exit(1);
	}
	DEBUG(1, "sent: %s\n", buf);
	rc = read(fd,buf,buflen-1);
	DEBUG(1, "Received (%d)\n", rc);
	if (rc < 0) {
		perror("reply error");
		exit(1);
	}
	if (rc == 0) {
		fprintf(stderr, "EOF on reply?\n");
		exit(1);
	}
	if (rc < buflen) {
		buf[rc] = 0;
	}
	/* should be "ACK Y" */
	if (0 == strcmp(buf,"ACK N")) {
		fprintf(stderr, "Psion refused the file, disk full?\n");
		exit(1);
	}
	if (0 != strcmp(buf,"ACK Y")) {
		fprintf(stderr, "Unexpected response: %s\n", buf);
		exit(1);
	}
	DEBUG(1, "ack received: %s\n", buf);

	cnt = 0;
	t0 = tx = t = time(NULL);

	while (!ferror(f) && !feof(f)) {
		int wrc;

		rc = fread(buf, 1, buflen, f);
		if (rc == 0) continue;

		wrc = write(fd,buf,rc);
		if (wrc < 0) {
			perror("write");
			exit(1);
		}
		if (wrc < rc) {
			fprintf(stderr, "Problem: only sent %d of %d\n", 
				wrc, rc);
			exit(1);
		}

		cnt += rc;
		if (verbose) {
			/* progress indication */
			t = time(NULL);
			if (t - t0 == 0 || cnt == 0 || tx == t)
				/* avoid division errors */
				/* only once per second */
				continue;
			tx = t;
			printf("sent %d/%lu bytes=%4.1f%% in %d sec," 
			       " %g Kbytes/s, to go %li sec  \r", 
			       cnt, s.st_size, 100.0 * cnt / s.st_size, t - t0,
			       cnt / 1000.0 / (t - t0), 
			       ( s.st_size - cnt ) * (t - t0) / cnt);
			fflush(stdout);
		}
	}
	if (ferror(f)) {
		perror("ferror");
		exit(1);
	}
	if (cnt != s.st_size) {
		printf("Warning: "
		       "file size changed: initial: %lu, actual: %d\n",
		       s.st_size, cnt);
	}

	/* Check for close on the other side */
	rc = read(fd,buf,buflen);
	if (rc > 0) {
		fprintf(stderr, "Strange: the other side responded.\n");
		fprintf(stderr, "rc=%d, data:%s\n", rc, buf);
		exit(1);
	}
	if (rc == 0) {
		fprintf(stderr, "Received end of file.\n");
	}
	if (rc == -1) {
		if (errno == EPERM) {
			/* Strange error code to get in this case */
			DEBUG(1, "Other side closed connection, OK\n");
		} else {
			perror("last read");
			exit(1);
		}
	}

	if (verbose) {
		t = time(NULL);
		if (t == t0) t++;	/* white lie for fast transfers */
		printf("\r%79s\r","");	/* Cleanup the progress line */
		printf("Sent %s, %d bytes in %d sec. %g KBytes/sec\n",
		       filename, cnt, t - t0, cnt / 1000.0 / (t - t0));
	}

	close(fd);
	return 0;
}

int handle_client(int cfd) {
	int buflen;
	char* buf;
	int rc;
	/* fields of file transfer header */
	unsigned int fsize;
	unsigned int fmode;
	unsigned int fdate1;
	unsigned int fdate2;
	char* fname;
	unsigned int fdate;
	unsigned long long int fdatell;
	FILE* f;
	int cnt;
	int t0, tx, t;

	buflen = irttp_get_mtu(cfd);
	DEBUG(1, "mtu=%d\n", buflen);
	if (buflen != 2045 && buflen != 2044) {
		fprintf(stderr, "Expecting mtu to be 2045, investigate!\n");
	}
	DEBUG(1, "Setting mtu to 2044\n");
	buflen = 2044;
	if (!(buf = malloc(buflen))) {
		perror("malloc");
		exit(1);
	}
	if (!(buf = malloc(buflen))) {
		fprintf(stderr, "malloc buf failed\n");
		exit(1);
	}

	/* Wait for the other side to send a header */
	/* 
	   Sample headers received:
	   DATA 185
	   FILE 55175 32 14689800 2691219200 Data
	        size mode datehi  datelo     name
	*/
	/*
	   This version only sends and receives files.
	   It is possible to extend this to other data types the
	   Psion can send and receive.
	*/

	rc = read(cfd, buf, buflen);
	if (rc < 0) {
		perror("1st read");
		exit(1);
	}
	if (rc == 0) {
		perror("1st read 0");
		exit(1);
	}
	assert(rc < buflen);
	buf[rc] = 0;
	DEBUG(1, "%s\n", buf);

	fsize = 0;
	fdate = 0;
	if (0 == strncmp(buf, "FILE ", 5)) {
		cnt = 0; /* to be safe */
		rc = sscanf(buf, "FILE %u %u %u %u %n", 
			    &fsize, &fmode, &fdate1, &fdate2,
			    &cnt);
		if (!(rc == 4 || rc == 5)) {
			/* grumble */
			fprintf(stderr, "sscanf \"%s\" rc=%d\n", buf, rc);
			exit(1);
		}
		assert(cnt < buflen);
		fname = strdup(buf+cnt);
		fdatell = ((unsigned long long int) fdate1 << 32) + fdate2;
		fdate = ( fdatell - 62168263200000000ULL) / 1000000 ;
		
		DEBUG(1, "filename: %s\n", fname);
		DEBUG(1, "filesize: %d\n", fsize);
		DEBUG(1, "Filemode: %d", fmode);
		DEBUG(1, "%s", (fmode &  1 ? ", Readonly" : ""));
		DEBUG(1, "%s", (fmode &  2 ? ", Hidden" : ""));
		DEBUG(1, "%s", (fmode & 32 ? ", Modified" : ""));
		DEBUG(1, "%s", (fmode & ~35 ? ", Unknown" : ""));
		DEBUG(1, "\n");
		DEBUG(1, "fdate1:   %u = 0x%x\n", fdate1, fdate1);
		DEBUG(1, "fdate2:   %u = 0x%x\n", fdate2, fdate2);
		DEBUG(1, "filedate: %Ld\n", fdatell);
		DEBUG(1, "filedate: %d = %s", fdate, 
		       asctime(gmtime((time_t*)&fdate)));

		if (!(f = fopen(fname,"wb"))) {
			perror(fname);
			exit(1);
		}
	} else if (0 == strncmp(buf, "DATA ", 5)) {
		rc = sscanf(buf, "DATA %d", &fsize);
		if (rc != 1) {
			fprintf(stderr, "sscanf rc=%d\n", rc);
			exit(1);
		}
		fname = strdup("/tmp/psion5-data");
		if (!(f = fopen(fname,"wb"))) {
			perror(fname);
			exit(1);
		}
	} else {
		fprintf(stderr, "Unknown data type: %s\n", buf);
		exit(1);
	}

	rc = write(cfd,"ACK Y",5);
	if (rc != 5) {
		perror("1st write");
		fprintf(stderr,"1st write rc = %d\n", rc);
		exit(1);
	}
			
	cnt = 0;
	t0 = tx = t = time(NULL);
	while (cnt < fsize) {
		int wrc;
		rc = read(cfd,buf,buflen);
		if (rc < 0) {
			perror("data read");
			/* EPERM on disconnect ? */
			exit(1);
		}
		if (rc == 0) {
			perror("data read 0");
			exit(1);
		}
		wrc = fwrite(buf,rc,1,f);
		if (wrc != 1) {
			perror("fwrite");
			exit(1);
		}
		cnt += rc;

		if (verbose) {
			/* progress indication */
			t = time(NULL);
			if (t - t0 == 0 || cnt == 0 || tx == t)
				/* avoid division errors */
				/* only once per second */
				continue;
			tx = t;
			printf("got %d/%u bytes=%g%% in %d sec,"
			       " %g Kbytes/s, to go %i sec  \r", 
			       cnt, fsize, 100.0 * cnt / fsize, t - t0,
			       cnt / 1000.0 / (t - t0), 
			       ( fsize - cnt ) * (t - t0) / cnt);
			fflush(stdout);
		}
	}
		
	if (cnt != fsize) {
		printf("Warning: "
		       "file size changed: initial: %u, actual: %d\n",
		       fsize, cnt);
	}
	if (verbose) {
		if (t == t0) t++;	/* white lie for fast transfers */
		printf("\r%79s\r","");	/* Cleanup the progress line */
		printf("Received %s, %d bytes in %d sec. %g KBytes/sec\n",
		       fname, cnt, t - t0, cnt / 1000.0 / (t - t0));
	}

	rc = fclose(f);
	if (rc != 0) {
		perror("fclose");
		exit(1);
	}
	if (fdate) {
		struct utimbuf utb;
		utb.actime = fdate;
		utb.modtime = fdate;
		rc = utime(fname,&utb);
		if (rc != 0) {
			perror(fname);
		}
	}
	free(fname);
	close(cfd);
	return 0;
}

int receivefile(int mode)
{
	/* 
	   The Psion 5 tries the following connections:
	   Epoc32:EikonIr:v1.0	IrDA:TinyTP:LsapSel
	   IrDA:IrCOMM       	Parameters
	   IrLPT              	IrDA:IrLMP:LsapSel
	   connect on 2

	   Warning: discovery reply after 101ms
	*/

	int addrlen = sizeof(struct sockaddr_irda);
	int fd;
	struct sockaddr_irda peer;
	int cfd;

        /* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }

	/* Bind local service */
	peer.sir_family = AF_IRDA;
	strcpy(peer.sir_name, "Epoc32:EikonIr:v1.0");
	peer.sir_lsap_sel = LSAP_ANY;
	
	if (bind(fd, (struct sockaddr*)&peer, sizeof(struct sockaddr_irda))) {
			perror("bind");
			return -1;
	}

	if (listen(fd, 2)) {
		perror("listen");
		return -1;
	}

	/* FIXME: allow more simultaneous clients */
	for (;;) {
		cfd = accept(fd, (struct sockaddr *) &peer, &addrlen);
		if (cfd < 0) {
			perror("accept");
			return -1;
		}

		if (handle_client(cfd))
			break;
	       
		if (mode == 1)
			break;
	}

	/* sleep(1); */
	close(fd);
	return 0;
}


int
main(int argc, char* argv[]) {
	char* argv0 = argv[0];

	if (argc <= 1) {
		fprintf(stderr, "Usage: %s [-S ms] [-D n] [-v] [-q] "
				"[-s file] [-r] [-b]\n", argv0);
		fprintf(stderr, "\t-s file\tSend file to the Psion\n");
		fprintf(stderr, "\t-r\tReceive a file from the Psion\n");
		fprintf(stderr, "\t-b\tReceive multiple files (batch mode)\n");
		fprintf(stderr, "\t-v\tVerbose mode, progress indication\n");
		fprintf(stderr, "\t-q\tquiet mode, no progress indication\n");
		fprintf(stderr, "\t-S ms\tSet discovery slot timeout,");
		fprintf(stderr, " standard 80ms, default 120ms\n");
		fprintf(stderr, "\t-D n\tDebug level\n");
		exit(1);
	}
	
	/* skip program name */
	argv++; argc--;

	for ( ; argc>0 && argv[0] ; argc-- , argv++) {
		if (0 == strcmp(argv[0],"-s")) {
			/* FIXME: sending multiple files does not
			   work yet. We need to wait for the user
			   to select receive again on the psion */
			if (argv[1] && 0 == strcmp(argv[1],"--")) {
				/* Allow the user to send ANY filename */
				argv++; argv++;
				for ( ; argv[1] ; argv++ , argc--) {
					sendfile(argv[1]);
				}
			} else {
				/* Send files upto next switch */
				while (argv[1] && *argv[1] != '-') {
					sendfile(argv[1]);
					argv++; argc--;
				}
			}
		} else if (argv[0] && 0 == strcmp(argv[0],"-D")) {
			debug_level = atoi(argv[1]);
			argv++; argc--;
		} else if (argv[0] && 0 == strcmp(argv[0],"-S")) {
			slot_timeout = atoi(argv[1]);
			argv++; argc--;
		} else if (argv[0] && 0 == strcmp(argv[0],"-r")) {
			receivefile(1);
		} else if (argv[0] && 0 == strcmp(argv[0],"-b")) {
			receivefile(0);
		} else if (argv[0] && 0 == strcmp(argv[0],"-v")) {
			verbose = 1;
		} else if (argv[0] && 0 == strcmp(argv[0],"-q")) {
			verbose = 0;
		} else {
			fprintf(stderr,"Error: unknown switch %s\n", argv[0]);
			fprintf(stderr,"Call without args for usage: %s\n", 
				argv0);
			exit(1);
		}
	}
	return 0;
}
