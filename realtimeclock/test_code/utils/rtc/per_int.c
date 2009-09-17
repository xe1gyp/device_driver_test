/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *      gcc -s -Wall -Wstrict-prototypes perinton.c -o perinton
 *
 * This binary is a part of RTC test suite.
 *
 * History:
 * Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
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

	int i;
	int fd;
	int retval;
	int irqcount;
	int count = 10;
	unsigned long data;

	/* Creating a file descriptor for RTC */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test\n\n");

	fprintf(stderr, "Enter the number of times to read periodic interrupts "
			"at 1 Hz frequency: ");
	fflush(stderr);
	scanf("%d", &count);
	if (count < 0) {
		fprintf(stderr, "Invalid number\n");
		_exit(0);
	}

	retval = ioctl(fd, RTC_UIE_ON, 0);
	if (retval == -1) {
		fprintf(stderr, "\n\t\t\t RTC_UIE_ON is selected \n\n");
		perror("RTC_UIE_ON, ioctl");
		_exit(errno);
	}

	/* Reading periodic interrupts using read system call */
	for (i = 1; i <= count; i++) {
		/* This read will block */
		retval = read(fd, &data, sizeof(unsigned long));
		if (retval == -1) {
			perror("read");
			_exit(errno);
		}
		irqcount++;
	}

	/* Reading periodic interrupts using select and read system call */

	printf("\nReading %d times using select and read system call: ", count);
	fflush(stderr);

	for (i = 1; i <= count; i++) {
		struct timeval tv = { 5, 0 };/* 5 second timeout on select */
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		/* The select will wait until an RTC interrupt happens. */
		retval = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (retval == -1) {
			perror("select");
			_exit(errno);
		}
		/* This read won't block unlike the select-less case above. */
		retval = read(fd, &data, sizeof(unsigned long));
		if (retval == -1) {
			perror("read");
			_exit(errno);
		}
		fprintf(stdout, " %d", i);
		fflush(stdout);
		irqcount++;
	}

	printf("\n\n");
	fflush(stderr);

	retval = ioctl(fd, RTC_UIE_OFF, 0);
	if (retval == -1) {
		fprintf(stderr, "\n\t\t\t RTC_UIE_OFF is selected \n\n");
		perror("RTC_UIE_OFF, ioctl");
		_exit(errno);
	}

	close(fd);
	return 0;
}
