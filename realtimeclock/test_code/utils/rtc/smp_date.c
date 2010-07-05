/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes -D_GNU_SOURCE -lpthread  smp_date.c -o smp_date
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
#include <sched.h>

#define MAXIMUM_TIME 1000000




void task1(int fd);
void task2(int fd);

int main(int argc, char *argv[])
{
	pthread_t thread1;
	pthread_t thread2;
	int ret;
	int fd;


	fprintf(stdout, "The test case RTC is running ...\n");


	/* Open Real Time Clock Device */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	/* Create the First thread */
	ret = pthread_create(&thread1, NULL, (void *)task1, (int *) fd);
	if (ret) {
		perror("Thread1: Error creating thread 1");
		exit(EXIT_FAILURE);
	}

	/* Create the Second thread */
	ret = pthread_create(&thread2, NULL, (void *)task2, (int *) fd);
	if (ret) {
		perror("Thread2: Error creating thread 1");
		exit(EXIT_FAILURE);
	}

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	close(fd);
	exit(EXIT_SUCCESS);
}

void task1(int fd)
{
	int return_ioctl;
	int i = 0;
	int temp_second = 0;
	struct rtc_time rtc_tm;
	cpu_set_t cpuset;
	pthread_attr_t attr;
	int result_aff;

	pthread_attr_init(&attr);

	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);
	result_aff = pthread_attr_setaffinity_np
			(&attr, sizeof(cpuset), &cpuset);
	if (result_aff != 0)
		fprintf(stdout,
			"Error setting the affinity...%d \n", result_aff);

	result_aff = pthread_attr_getaffinity_np
			(&attr, sizeof(cpuset), &cpuset);
	if (result_aff != 0)
		fprintf(stdout,
			"Error setting the affinity...%d \n", result_aff);

	fprintf(stdout, "cpuset...%d \n", cpuset);
	fprintf(stdout, "Thread1: Starting...\n");

	while (i < MAXIMUM_TIME) {


		/* Reading Current RTC Date/Time */
		return_ioctl = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (return_ioctl == -1) {
			perror("Thread1: Error RTC_RD_TIME ioctl!\n");
			_exit(errno);
		}

		/* */
		if (temp_second != rtc_tm.tm_sec) {
			fprintf(stdout, "Thread1: Counter %d:\
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
	fprintf(stdout, "Thread1: Finished!\n");
}

void task2(fd)
{
	int return_ioctl;
	int return_mutex;
	int nolock = 0;
	int i = 0;
	int temp_second = 0;
	struct rtc_time rtc_tm;
	pthread_attr_t attr;
	cpu_set_t cpuset;
	int result_aff;

	pthread_attr_init(&attr);
	pthread_attr_getaffinity_np(&attr, sizeof(cpuset), &cpuset);
	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	result_aff = pthread_attr_setaffinity_np
			(&attr, sizeof(cpuset), &cpuset);
	if (result_aff != 0)
		fprintf(stdout, "Error setting\
				the affinity...%d \n", result_aff);
	else
		fprintf(stdout, "Could set the affinity... \n");

	pthread_attr_getaffinity_np(&attr, sizeof(cpuset), &cpuset);
	fprintf(stdout, "cpuset...%d \n", cpuset);
	fprintf(stdout, "Thread2: Starting...\n");

	while (i < MAXIMUM_TIME) {

		/* Reading Current RTC Date/Time */
		return_ioctl = ioctl(fd, RTC_RD_TIME, &rtc_tm);
			if (return_ioctl == -1) {
				perror("Thread2: Error RTC_RD_TIME ioctl!\n");
				_exit(errno);
		}

			/* */
			if (temp_second != rtc_tm.tm_sec) {

				fprintf(stdout, "Thread2: Counter %d\
						: %d-%d-%d %02d:%02d:%02d\n",
					i,
					rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
					rtc_tm.tm_year + 1900,
					rtc_tm.tm_hour, rtc_tm.tm_min,
					rtc_tm.tm_sec);
				fflush(stdout);
			/*	temp_second = rtc_tm.tm_sec;*/
			}

			/* */
			i++;
	}
	fprintf(stdout, "Thread2: Finished!\n");
	fprintf(stdout, "Thread2: Number of times\
			missed the lock: %d\n", nolock);
}


