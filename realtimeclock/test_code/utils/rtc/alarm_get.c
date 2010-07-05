/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes get_alarm.c -o getalarm
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

	int fd, retval, irqcount = 0;
	unsigned long data;
	struct rtc_time rtc_tm;
	int alarm_time;

	/* Creating a file descriptor for RTC */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test\n\n");
	fflush(stderr);

	/* Read the RTC time / date
	 * The ioctl command RTC_RD_TIME is used to read the current timer
	 * RTC_RD_TIME: This ioctl needs one argument(struct rtc_time *),
	 * and it can be used to get the current RTC time.
	 */

	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr,
		"\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	fflush(stderr);

	/* Set the alarm to 5 sec in the future, and check for rollover */
	rtc_tm.tm_sec += 60;
	if (rtc_tm.tm_sec >= 60) {
		rtc_tm.tm_sec %= 60;
		rtc_tm.tm_min++;
	}
	if (rtc_tm.tm_min == 60) {
		rtc_tm.tm_min = 0;
		rtc_tm.tm_hour++;
	}
	if (rtc_tm.tm_hour == 24)
		rtc_tm.tm_hour = 0;

	retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, "alarm setting done...\n");
	fflush(stderr);

	/* Read the current alarm settings */
	retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, "Alarm time now set to %02d:%02d:%02d\n",
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	fflush(stderr);

	/* Enable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_ON, 0);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, "Waiting 5 seconds for alarm");
	fflush(stderr);

	/* This blocks until the alarm ring causes an interrupt
	 * retval = read(fd, &data, sizeof (unsigned long));
	 */
	if (retval == -1) {
		perror("read");
		_exit(errno);
	}
	irqcount++;
	fprintf(stderr, "Alarm Rang\n");
	fflush(stderr);

	/* Disable alarm interrupts
	 * retval = ioctl(fd, RTC_AIE_OFF, 0);
	 */
	if (retval == -1) {
		perror("Alarm interrupts not disabled!");
		_exit(errno);
	}

	close(fd);
	return 0;

}
