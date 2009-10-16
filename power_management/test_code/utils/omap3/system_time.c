/*
 * OMAP3 System Time  test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Teerth Reddy <teerth@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 *
 * 2009-10-16: Teerth Reddy      Initial code
 *
 */


#include <sys/time.h>
#include <stdio.h>

int main()
{
	struct timeval t1;
	struct timeval t2;
	int t_diff;
	gettimeofday(&t1, NULL);
	sleep(600);
	gettimeofday(&t2, NULL);
	t_diff = t2.tv_sec - t1.tv_sec;
	if (t_diff != 600)
		printf("Test FAIL\n");
	else
		printf("Test PASS\n");
}

