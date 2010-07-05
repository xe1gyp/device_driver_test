/*
 *Test Code for Real Time Clock Driver
 *
 *Compile with:
 *	gcc -s -Wall -Wstrict-prototypes alarm_reset.c -o alarmreset
 *This binary is a part of RTC test suite.
 *
 *History:
 *Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
 *
 * XX-XX-XXXX	Texas Instruments	Initial version of the testcode
 * 12-09-2008	Ricardo Perez Olivares	Adding basic comments, variable
 *					names according to coding
 *					standars.
 *
 * Copyright (C) 2004-2009 Texas Instruments, Inc
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int
main(int argc, char *argv[])
{

	int fd, retval; 

	/* Creating a file descriptor for RTC*/
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test\n\n");
	fflush(stderr);

	/* Disabling alarm interrupts using RTC_AIE_OFF */
	retval = ioctl(fd, RTC_AIE_OFF, 0);
	if (retval == -1) {
		printf("Alarm interrupts not disabled\n");
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, "Successful alarm reset!\n\n");
	fflush(stderr);
	
	close(fd);
	return 0;

}
