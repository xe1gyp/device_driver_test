/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes rtc_read.c -o rtc_read
 *
 * This binary is a part of RTC test suite.
 *
 * History:
 *
 * Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
 *
 * XX-XX-XXXX	Texas Instruments	Initial version of the testcode
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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#define MAXIMUM_TIME 10000


int main(int argc, char *argv[])
{
	int ret;
	int fd;
	int i = 0;
	int return_ioctl;
	int temp_second = 0;
	struct rtc_time rtc_tm;

	fprintf(stdout, "The test case RTC is running ...\n");
	/* Open Real Time Clock Device */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	while (i < MAXIMUM_TIME) {
		/* Reading Current RTC Date/Time */
		return_ioctl = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (return_ioctl == -1) {
			perror("RTC: Error RTC_RD_TIME ioctl!\n");
			_exit(errno);
		}

		if (temp_second != rtc_tm.tm_sec) {
			fprintf(stdout, "RTC: Counter %d:\
					%d-%d-%d %02d:%02d:%02d\n",
				i,
				rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
				rtc_tm.tm_year + 1900,
				rtc_tm.tm_hour, rtc_tm.tm_min,
				rtc_tm.tm_sec);
				fflush(stdout);
		/*		temp_second=rtc_tm.tm_sec; */
		}
		i++;
	}

	close(fd);
	exit(EXIT_SUCCESS);
}



