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

int
main(void)
{

	int fd, retval;
	int choice = 0;

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test \n\n");

	fprintf(stderr, "Enter the Periodic IRQ update rate:\n");
	fprintf(stderr, "Allowed Values : \n");
	fprintf(stderr, "1 - updates every second\n");
	fprintf(stderr, "2 - updates every minute\n");
	fprintf(stderr, "4 - updates every hour\n");
	fprintf(stderr, "8 - updates every day\n");
	fprintf(stderr, "Enter your choice:");
	scanf("%d",&choice);
	if ( choice < 1 || choice > 8 ){
		fprintf(stderr, "Invalid Value!!!\n");
		_exit(1);
	}
	

	retval = ioctl(fd,RTC_IRQP_SET, choice);
	if ( retval == -1 ){
		fprintf(stderr, "\n\t\t\t RTC_IRQP_SET is selected \n\n");
		perror("RTC_IRQP_SET, ioctl");
		_exit(errno);
	}
	retval = ioctl(fd, RTC_UIE_ON, 0);
	if (retval == -1) {
		fprintf(stderr, "\n\t\t\t RTC_UIE_SET is selected \n\n");
		perror("RTC_UIE_ON, ioctl");
		_exit(errno);
	}

	printf("Periodic update interrupts are enabled successfully.\n");
	printf("Use ./getperint1 or ./getperint2 prograrm to get interrupt"
				"information from TWL4030 RTC module\n");
	printf("Use perintoff to turn off periodic update interrupts\n");
		close(fd);
	return 0;

} /* end main */
