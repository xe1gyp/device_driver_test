/*
 * OMAP3 System Time  test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Teerth Reddy <teerth@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 *
 * 2009-10-16: Teerth Reddy      Initial code
 *
 */


#include <sys/time.h>
#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int fd, retval, t_diff_min, t_diff_sec;
	struct rtc_time rtc_tm1, rtc_tm2;

	/* Creating a file descriptor for RTC */
	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	/* Reading Current RTC Date/Time */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm1);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	//sleep(600);
	sleep(60);

	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm2);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	t_diff_min = rtc_tm2.tm_min - rtc_tm1.tm_min;
	t_diff_sec = rtc_tm2.tm_sec - rtc_tm1.tm_sec;

	printf("T1 value min= %d sec =%d \n", rtc_tm1.tm_min, rtc_tm1.tm_sec);
	printf("T2 value min= %d sec =%d \n", rtc_tm2.tm_min, rtc_tm2.tm_sec);

	if ((t_diff_min == 1) && (t_diff_sec == 0)) {
		printf("TEST PASSED\n");
		return(0);
	} else {
		printf("TEST FAILED\n");
		return(1);
	}

}

