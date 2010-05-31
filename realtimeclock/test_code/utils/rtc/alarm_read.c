/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes alarm_read.c -o alarm_read
 * This binary is a part of RTC test suite.
 *
 * History:
 * Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
 *
 * Copyright (C) 2004-2010 Texas Instruments, Inc
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

/* This function shows the menu when the alarm_read is running */
void show_usage(void)
{
	printf("Set alarm time options\n");
	printf("alarm time is set to 15 secs from current time repeatedly 'N' times\n");
	printf("Enter the value of N:");
}

int main(int argc, char *argv[])
{

	int fd, retval;
	struct rtc_time rtc_tm;
	struct rtc_time rtc_almtm;
	int choice;
	int mismatch_err = 0;
	int count;
	int status = 0;

	/* Creating a file descriptor for RTC */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test\n\n");
	fflush(stderr);

	/* Displaying usage */
	show_usage();
	scanf("%d", &choice);

	for (count = 0; count < choice; count++) {
		/*
		 * default alarm time is set to 15 secs from current time
		 */

		/*
		 * The ioctl command RTC_RD_TIME is used to read the current
		 * time.
		 * RTC_RD_TIME: This ioctl needs one argument
		 * (struct rtc_time *),
		 * and it can be used to get the current RTC time.
		 */

		retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		/*
		 * Setting the alarm to 15 sec in the future and
		 * check for rollover
		 */

		rtc_tm.tm_sec += 15;
		/* Evalulating 60 seconds as 1 minute */
		while (rtc_tm.tm_sec >= 60) {
			rtc_tm.tm_sec -= 60;
			rtc_tm.tm_min++;
		}
		/* Evaluating 60 min as 1 hour */
		while (rtc_tm.tm_min >= 60) {
			rtc_tm.tm_min -= 60;
			rtc_tm.tm_hour++;
		}
		/* Evaluating 24 hours as 1 day */
		while (rtc_tm.tm_hour >= 24) {
			rtc_tm.tm_hour -= 24;
			rtc_tm.tm_mday++;
		}

		retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		fprintf(stderr,
		"\nAlarm Set at "
		" %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
		rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
		rtc_tm.tm_sec);

		fflush(stderr);

		/* Read the current alarm settings */
		retval = ioctl(fd, RTC_ALM_READ, &rtc_almtm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		/* check if set and read alarm time are same */
		if ((rtc_almtm.tm_hour == rtc_tm.tm_hour) &&
			(rtc_almtm.tm_min == rtc_tm.tm_min) &&
			(rtc_almtm.tm_sec == rtc_tm.tm_sec) &&
			(rtc_almtm.tm_mday == rtc_tm.tm_mday) &&
			(rtc_almtm.tm_mon == rtc_tm.tm_mon) &&
			(rtc_almtm.tm_year == rtc_tm.tm_year)) {
			fprintf(stderr, "Alarm time verified\n");
		} else {
			mismatch_err++;
			fprintf(stderr, "Alarm time programming mismatch\n");
			fprintf(stderr,
			 "Alarm time readback value is "
			 "%d-%d-%d : %02d:%02d:%02d\n",
			 rtc_almtm.tm_mday, rtc_almtm.tm_mon+1,
			 rtc_almtm.tm_year+1900, rtc_almtm.tm_hour,
			 rtc_almtm.tm_min, rtc_almtm.tm_sec);
		}

	}
	if (mismatch_err > 0)
		status = 1;

	/* Enable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_ON, 0);

	fflush(stderr);

	close(fd);
	return status;

}
