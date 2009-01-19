/*
 * Test Code for Watchdog Driver
 *
 * This program tests "write" call supported by watchdog timer driver.
 * This program opens "/dev/watchdog" and calls write in a while loop
 * thereby resetting watchdog timer.
 *
 * Compile with:
 * gcc -s -Wall -Wstrict-prototypes testwdtwritel.c -o testwdtwrite
 *
 * This binary is a part of Watchdog test suite.
 *
 * History:
 *
 * 19-02-2004   Texas Instruments       Initial version of the testcode
 * 12-09-2008   Ricardo Perez Olivares  Adding basic comments, variable
 *					names according to coding
 * 					standars.
 *
 * Copyright (C) 2004-2009 Texas Instruments, Inc
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
        int ret_val;

        int fd=open("/dev/watchdog",O_WRONLY);
        if (fd==-1) {
                perror("watchdog");
                return 1;
        }

	data = 2;
        ret_val = ioctl (fd, WDIOC_SETTIMEOUT, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_SETTIMEOUT failed");
        }
        else {
	        printf ("\nNew timeout value is : %d seconds", data);
        }

        ret_val = ioctl (fd, WDIOC_GETTIMEOUT, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_GETTIMEOUT failed");
        }
        else {
	        printf ("\nCurrent timeout value is : %d seconds\n", data);
        }

        while(1) 
        {
                write(fd, "\0", 1);
                sleep(1);
        }
        close (fd);
	
	return 0;
}
