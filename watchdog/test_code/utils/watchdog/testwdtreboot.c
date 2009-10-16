/*
 * Test Code for Watchdog Driver
 *
 * This program tests the setting of interval ioctls supported by watchdog
 * timer driver. This program opens "/dev/watchdog" and calls the various
 * ioctls supported by watchdog timer driver and prints the values. At the
 * end it sets the timeout for watchdog timer and allows timeout to exprire
 * by sleeping more than timeout period.
 *
 * Compile with:
 * gcc -s -Wall -Wstrict-prototypes testwdtioctl.c -o testwdtioctl
 *
 * This binary is a part of Watchdog test suite.
 *
 * History:
 *
 * 10-15-2009	Vimal Singh	Initial version of the testcode
 *
 * Copyright (C) 2009 Texas Instruments, Inc
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
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
#include <asm/types.h>
#include <linux/watchdog.h>
#include <sys/stat.h>

int main(int argc, const char *argv[]) {
	int data = 0;
	int data2 = 0;
	int ret_val;
	struct watchdog_info ident;

	int fd = open("/dev/watchdog", O_WRONLY);
	if (fd == -1) {
		perror("watchdog");
		return 1;
        }

	if (argc == 2)
		sscanf(argv[argc-1], "%d",&data2 );
	else
		data2 = 2;

	ret_val = ioctl(fd, WDIOC_GETSUPPORT, &ident);
	if (ret_val) {
		printf("\nWatchdog Timer : WDIOC_GETSUPPORT failed");
	} else {
		printf("\nIdentity : %s", ident.identity);
		printf("\nFirmware_version : %d", ident.firmware_version);
		printf("\nOptions : 0x%x", ident.options);
	}

	ret_val = ioctl(fd, WDIOC_GETSTATUS, &data);
	if (ret_val)
		printf("\nWatchdog Timer : WDIOC_GETSTATUS failed");
	else
		printf("\nget Status : %x", data);

	ret_val = ioctl(fd, WDIOC_GETBOOTSTATUS, &data);
	if (ret_val)
		printf("\nWatchdog Timer : WDIOC_GETBOOTSTATUS failed");
	else
		printf("\nBoot Status : %x", data);

	ret_val = ioctl(fd, WDIOC_GETTIMEOUT, &data);
	if (ret_val)
		printf("\nWatchdog Timer : WDIOC_GETTIMEOUT failed");
	else
		printf("\nCurrent timeout value before settime is :"
						"%d seconds\n", data);

	ret_val = ioctl(fd, WDIOC_SETTIMEOUT, &data2);
	if (ret_val)
		printf("\nWatchdog Timer : WDIOC_SETTIMEOUT failed");
	else
		printf("\nNew timeout value is : %d seconds", data2);

	ret_val = ioctl(fd, WDIOC_GETTIMEOUT, &data);
	if (ret_val)
		printf("\nWatchdog Timer : WDIOC_GETTIMEOUT failed");
	else
		printf("\nCurrent timeout value is : %d seconds\n", data);

	sleep(data2 + 1);

	return 0;
}
