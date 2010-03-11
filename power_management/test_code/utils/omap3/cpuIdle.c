/*
 * OMAP3 cpuidle  test file
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
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
 * 2010-03-09: David Miramontes      Initial code
 *
 */

#include <stdio.h>
#include <string.h>

/* Valid C states */
#define C1 (2+2)
#define C2 (10+10)
#define C3 (50+50)
#define C4 (1500+1800)
#define C5 (2500+7500)
#define C6 (3000+8500)
#define C7 (4000+9000)
#define C8 (8000+25000)
#define C9 (10000+30000)

/* Invalid C states */
#define C10 (2+2-4)
#define C11 (10+10-30)
#define C12 (50+500)
#define C13 (2+3)
#define C14 (12+12)
#define C15 (1+1)
#define C16 (5000000)
#define C17 (80000+250000)
#define C18 (100000+300000)

int main(int argc, char *argv[])
{
	FILE *fp;
	int latency = C5;
	char *filename = "/dev/cpu_dma_latency";
	size_t size;
	if (argc <= 1) {
		printf("\n Usage:\n  cpuidle Cn where Cn = \n\n");
		printf(" Example:  ./cpuidle C5 \n\n");
		printf("C1 = MPU ON, CORE ON \n");
		printf("C2 = MPU ON, CORE ON \n");
		printf("C3 = MPU RET, CORE ON \n");
		printf("C4 = MPU OFF, CORE ON \n");
		printf("C5 = MPU RET, CORE RET \n");
		printf("C6 = MPU OFF, CORE RET \n");
		printf("C7 = MPU OSWR, CORE OSWR \n");
		printf("C8 = MPU OFF, CORE OSWR \n");
		printf("C9 = MPU OFF, CORE OFF \n\n");
		printf("C10 to C18 = Invalid values \n\n");
		return 1;
	}
#if 1
	if (strcmp(argv[1], "C1") == 0)
		latency = C1;
	else if (strcmp(argv[1], "C2") == 0)
		latency = C2;
	else if (strcmp(argv[1], "C3") == 0)
		latency = C3;
	else if (strcmp(argv[1], "C4") == 0)
		latency = C4;
	else if (strcmp(argv[1], "C5") == 0)
		latency = C5;
	else if (strcmp(argv[1], "C6") == 0)
		latency = C6;
	else if (strcmp(argv[1], "C7") == 0)
		latency = C7;
	else if (strcmp(argv[1], "C8") == 0)
		latency = C8;
	else if (strcmp(argv[1], "C9") == 0)
		latency = C9;
	else if (strcmp(argv[1], "C10") == 0)
		latency = C10;
	else if (strcmp(argv[1], "C11") == 0)
		latency = C11;
	else if (strcmp(argv[1], "C12") == 0)
		latency = C12;
	else if (strcmp(argv[1], "C13") == 0)
		latency = C13;
	else if (strcmp(argv[1], "C14") == 0)
		latency = C14;
	else if (strcmp(argv[1], "C15") == 0)
		latency = C15;
	else if (strcmp(argv[1], "C16") == 0)
		latency = C16;
	else if (strcmp(argv[1], "C17") == 0)
		latency = C17;
	else if (strcmp(argv[1], "C18") == 0)
		latency = C18;

	fp = fopen(filename, "r+");
	if (!fp) {
		perror("Unable to open PM Qos file");
		return -1;
	}

	size = fwrite(&latency, sizeof(latency), 1, fp);
	if (size != 1) {
		perror("Incomplete write");
		goto out;
	}

	fflush(fp);

	printf("Wrote %d to %s\n", latency, filename);

	sleep(4);

out:
	fclose(fp);

#endif
}
