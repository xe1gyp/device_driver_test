/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes alarm_set.c -o alarmset
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

void show_usage(void);
int validate_data(int fd, struct rtc_time *rtc_almtm);
int get_timedate(struct rtc_time *rtc_tm);
int show_menu(void);

int main(int argc, char *argv[])
{

	int fd, retval; 
	struct rtc_time rtc_tm, rtc_almtm;
	int choice, secs = 0;

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

	/* Getting usage */
	scanf("%d", &choice);

	/*
	 * Choice variable can take one of 3 available values
	 * 0 -> default alarm time is set to 5 secs from current time
	 * 1 -> Enter alarm time in secs:
	 * 2 -> Enter alarm date(format : MMDDYY) and time(format : HHMMSS)
	 */

	if (choice == 0 || choice == 1) {
		/* Read the RTC time/date */
		/*If choice 1 selected user set alarm seconds*/
		if ( choice == 1 ){
			fprintf(stderr, "Enter alarm time in seconds:");
			fflush(stderr);
			scanf("%d",&secs);
			if ( secs <= 0){
				fprintf(stderr, "Invalid alarm time\n");
				fflush(stderr);
				_exit(-1);
			}
		}

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

		fprintf(stderr,
			"\n\nCurrent RTC date/time is"
			" %d-%d-%d, %02d:%02d:%02d.\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
			rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
			rtc_tm.tm_sec);
		fflush(stderr);

		/*
		 * Setting the alarm to 5 sec in the future and
		 * check for rollover
		 */

		if ( choice == 0 )
			rtc_tm.tm_sec += 5;
		else
			rtc_tm.tm_sec += secs;
		/* Evalulating 60 seconds as 1 minute */
		while ( rtc_tm.tm_sec >= 60 ){
			rtc_tm.tm_sec -= 60;
			rtc_tm.tm_min++;
		}
		/* Evaluating 60 min as 1 hour */
		while ( rtc_tm.tm_min >= 60 ){
			rtc_tm.tm_min -= 60;
			rtc_tm.tm_hour++;
		}
		/* Evaluating 24 hours as 1 day */
		while ( rtc_tm.tm_hour >= 24 ){
			rtc_tm.tm_hour -= 24;
			rtc_tm.tm_mday++;
		}

		retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		fprintf(stderr, "Alarm setting done!\n");
		fflush(stderr);

		/* Read the current alarm settings */
		retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		fprintf(stderr, "Alarm time now set to %02d:%02d:%02d\n",
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	} else if (choice == 2) {
		get_timedate(&rtc_almtm);
		if (validate_data(fd, &rtc_almtm) < 0 ){
			fprintf(stderr, "Invalid alarm value!\n");
			fflush(stderr);
		}
		retval = ioctl(fd, RTC_ALM_SET, &rtc_almtm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		fprintf(stderr, "Alarm setting done!\n");
		fflush(stderr);

		/* Read the current alarm settings */
		retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		fprintf(stderr,
			"\n\nAlarm date and time now set to %d-%d-%d "
						"%02d:%02d:%02d\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
			rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
			rtc_tm.tm_sec);
		fflush(stderr);

	}
	} else {
		fprintf(stderr, "Invalid choice!\n");
		fflush(stderr);
		_exit(-1);
	}

	/* Enable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_ON, 0);

	if ( choice == 1 )
		fprintf(stderr, "Run ./alarm_get_event with in %d seconds"
				" to receive alarm notification\n", secs);
		fflush(stderr);
	
	close(fd);
	return 0;

}

/* This function shows the menu when the alarm_set is running */
void show_usage(void)
{
	printf("Set alarm time options\n");
	printf("0 -> default alarm time is set to 5 secs from current time\n");
	printf("1 -> Enter alarm time in secs:\n");
	printf("2 -> Enter alarm date(format : MMDDYY) and time"
						"(format : HHMMSS)\n");
	printf("Choice:");
}

/* This function get the time / date from user input when choice= 2 */
int get_timedate(struct rtc_time *rtc_tm)
{
	char date_str[7] = { 0 };
	char time_str[7] = { 0 };
	int num;
	unsigned char data[6] = { 0 };
	int i;

	show_menu();
	printf("Enter Date:");
	scanf("%s", date_str);
	printf("Enter Time:");
	scanf("%s", time_str);
	num = atoi(date_str);
	for (i = 0; i < 3; i++) {
		data[i] = num % 10;
		num /= 10;
		data[i] += (num % 10) * 10;
		num /= 10;
		/* printf ("data[%d] : %d\n", i, data[i]); Old comment,
		 * remove it?
		 */
	}
	num = atoi(time_str);
	for (; i < 6; i++) {
		data[i] = num % 10;
		num /= 10;
		data[i] += (num % 10) * 10;
		num /= 10;
		/* printf ("data[%d] : %d\n", i, data[i]);
		 * Old comment, remove it?
		 */
	}
	rtc_tm->tm_mday = data[1];
	rtc_tm->tm_mon = data[2] - 1;
	rtc_tm->tm_year = data[0] + 100;
	rtc_tm->tm_hour = data[5];
	rtc_tm->tm_min = data[4];
	rtc_tm->tm_sec = data[3];
	return 0;

}

/* This function validates the data using RTC_RD_TIME ioctl
 and rtc_time struct */
int validate_data(int fd, struct rtc_time *rtc_almtm)
{
	struct rtc_time rtc_tm;
	int retval;
	int time_nosecs, almtime_nosecs;
	/*
	 * The ioctl command RTC_RD_TIME is used to read the current timer
	 * RTC_RD_TIME:	This ioctl needs one argument(struct rtc_time *),
	 * and it can be used to get the current RTC time.
	 */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
		/*If RTC_RD_TIME can't be readed send an ioctl err*/
	}

	fprintf(stderr,
		"\n\nCurrent RTC date/time is %d-%d-%d %02d:%02d:%02d\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
		rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
		rtc_tm.tm_sec);
	fflush(stderr);
	time_nosecs =
	    rtc_tm.tm_sec + rtc_tm.tm_min * 60 + rtc_tm.tm_hour * 24 +
	    365 * rtc_tm.tm_year;
	almtime_nosecs =
	    rtc_almtm->tm_sec + rtc_almtm->tm_min * 60 +
	    rtc_almtm->tm_hour * 24 + 365 * rtc_almtm->tm_year;
	if (almtime_nosecs <= time_nosecs)
		return -1;
	else
		return 0;
}

/* Function to show menu where user can see how to put time and date format*/
int show_menu(void)
{
	int i = 0;
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	printf("*\tSet Alarm Date and Time Demo\t");
	printf("\n");
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	printf("*\tUsage : Date format MMDDYY\n");
	printf("*\tUsage : Time format HHMMSS\n");
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	return 0;
}
