/*
 * OMAP3 vfp  test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 * Vishwanath Sripathy <vishwanath.bs@ti.com>
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
 * 2010-03-15: Vishwanath Sripathy	Initial code
 *
 **/

#include <stdio.h>

int main(void)
{
	float a = 1.123;
	float b = 2.234;
	asm volatile ("mov r0, %0" : : "r"(a) : "r0", "r1", "r2");
	asm volatile ("mov r1, %0" : : "r"(b) : "r0", "r1", "r2");
	asm volatile ("fmsrr {s0, s1}, r0, r1");
	asm volatile ("fmsrr {s2, s3}, r0, r1");

	while (1) {
		asm volatile ("fadds s4, s0, s1");
		asm volatile ("fadds s0, s0, s2");
		asm volatile ("fadds s1, s1, s2");
		asm volatile ("fmrrs %0, r2, {s4, s5}" : "=r"(a) : : \
						"r0", "r1", "r2");
		printf("Result = %f\n", a);
		sleep(2);
	}
	return 0;
}

