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


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <linux/rtc.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int fd, retval;
	unsigned int sleep_time;
	int t_diff_hrs, t_diff_min, t_diff_sec;
	int rtc_diff_hrs, rtc_diff_min, rtc_diff_sec;

	struct rtc_time rtc_tm1, rtc_tm2;

	if (argc != 2) {
		printf("\nUsage:\t %s <sleep time in Sec>\n\n", argv[0]);
		exit(1);
	}

	sleep_time = strtoul(argv[1], NULL, 10);
	t_diff_hrs = sleep_time / 3600;
	if (t_diff_hrs > 48) {
		printf("\nSleep time should be less than 48 hrs");
		exit(1);
	}
	t_diff_min = (sleep_time / 60) - (t_diff_hrs * 60);
	t_diff_sec = sleep_time % 60;

	printf("Sleep Time: %d:%d.%d\n", t_diff_hrs, t_diff_min, t_diff_sec);

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

	sleep(sleep_time);

	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm2);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	rtc_diff_hrs = rtc_tm2.tm_hour - rtc_tm1.tm_hour;
	rtc_diff_min = rtc_tm2.tm_min - rtc_tm1.tm_min;
	rtc_diff_sec = rtc_tm2.tm_sec - rtc_tm1.tm_sec;

	printf("T1 value min= %d:%d.%d \n", rtc_tm1.tm_hour, rtc_tm1.tm_min, rtc_tm1.tm_sec);
	printf("T2 value min= %d:%d.%d \n", rtc_tm2.tm_hour, rtc_tm2.tm_min, rtc_tm2.tm_sec);

	if ((t_diff_hrs == rtc_diff_hrs) && (t_diff_min == rtc_diff_min) && (t_diff_sec == rtc_diff_sec)) {
		printf("TEST PASSED\n");
		return(0);
	} else {
		printf("TEST FAILED\n");
		return(1);
	}
}
