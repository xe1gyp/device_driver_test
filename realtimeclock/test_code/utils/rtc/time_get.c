/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *      gcc -s -Wall -Wstrict-prototypes gettime.c -o gettime
 *
 * This binary is a part of RTC test suite.
 *
 * History:
 * Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
 *
 * XX-XX-XXXX   Texas Instruments       Initial version of the testcode
 * 12-09-2008   Ricardo Perez Olivares  Adding basic comments, variable
 *                                      names according to coding
 *                                      standars.
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

int main(int argc, char *argv[])
{

	int fd, retval;
	struct rtc_time rtc_tm;

	/* Creating a file descriptor for RTC */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	/* Reading Current RTC Date/Time */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}
	fprintf(stdout, "\nCurrent RTC Date/Time: %d-%d-%d %02d:%02d:%02d\n\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	fflush(stdout);
	fflush(stdout);
	fflush(stdout);
	close(fd);
	return 0;
}
