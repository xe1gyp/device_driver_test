
/*
 *Test Code for Real Time Clock Driver
 *
 *Compile with:
 *	gcc -s -Wall -Wstrict-prototypes alarm_reset.c -o alarmreset
 *This binary is a part of RTC test suite.
 *
 * History:
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

	int fd, retval; 

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test Example.\n\n");

	/* Read the RTC time/date */

	/* Disable alarm interrupts */
	/*RTC_AIE_OFF disable the alarm interrupts*/
	retval = ioctl(fd, RTC_AIE_OFF, 0);

	if (retval == -1) {
		printf(" alarm interrupts not disabled\n");
		perror("ioctl");
		_exit(errno);
	}


	printf("alarm reset successful...\n");
	
	close(fd);
	return 0;

}/* end main */
