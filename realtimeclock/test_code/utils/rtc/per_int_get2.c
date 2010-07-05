/*
 * Test Code for Real Time Clock Driver
 *
 * Compile with:
 *	gcc -s -Wall -Wstrict-prototypes getperint2.c -o getperint2
 *
 * This binary is a part of RTC test suite.
 *
 * History:
 * Copyright (C) 1996, Paul Gortmaker. This version is based on Paul's
 *
 * XX-XX-XXXX	Texas Instruments	Initial version of the testcode
 * 12-09-2008	Ricardo Perez Olivares	Adding basic comments, variable
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

	int i, fd, retval, irqcount = 0;
	unsigned long tmp, data;
	int choice = 0, count;
	char *choice_data[4] = {"second", "minute", "hour", "day"};

	/* Creating a file descriptor for RTC */
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Requested device cannot be opened!");
		_exit(errno);
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test\n\n");

	fprintf(stderr, "Enter number of updates:");
	scanf("%d", &count);
	if (count < 0) {
		fprintf(stderr, "Invalid number\n");
		_exit(0);
	}
	fprintf(stderr, "Counting %d update (1/%s) interrupts\n",
		count, choice_data[choice]);
	fflush(stderr);

	/* Read periodic IRQ rate */
	retval = ioctl(fd, RTC_IRQP_READ, &tmp);

	if (retval == -1) {
		printf(" not able to read periodic irq rate\n");
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, " Update interrupts (one per %s)\n", choice_data[tmp]);
	fprintf(stderr, "\nusing select and read system call on /dev/rtc0:\n");
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

	printf("\n");
	close(fd);
	return 0;

}
