/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes getalarm_event.c -o getalarm_event
 *
 * This binary is a part of RTC test suite.
 *
 * History:
 * Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
 *
 * XX-XX-XXXX	Texas Instruments	Initial version of the testcode
 * 12-11-2008	Ricardo Perez Olivares	Adding basic comments, variable
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
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{

	int fd, retval;
	unsigned long data;
	struct rtc_time rtc_tm;

	/* Creating a file descriptor for RTC in /dev/rtc0*/
	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		fprintf(stdout, "Error...!!! /dev/rtc0 not present.");
		return 1;
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test Example.\n\n");

	/* Read the current alarm settings
	 * RTC_ALM_READ:  This ioctl needs one argument(struct rtc_time *),
	 * and it can be used to get the current RTC alarm
	 * parameter.
	 */

	retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	/* Calling for members of rtc_tm structure, to evaluate
	 * if the RTC_ALM_READ contains a valid data, if not shows an error
	 */
	fprintf(stderr,
			"\n\nWaiting for alarm at date and time  %d-%d-%d"
							"%02d:%02d:%02d.\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
			rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
			rtc_tm.tm_sec);
		
	fflush(stderr);

	/* This blocks until the alarm ring causes an interrupt */

	retval = read(fd, &data, sizeof (unsigned long));
	if (retval == -1) {
		perror("read");
		_exit(errno);
	}
	fprintf(stdout, " okay. Alarm rang.\n");

	close(fd);
	return 0;

} /* end main */

