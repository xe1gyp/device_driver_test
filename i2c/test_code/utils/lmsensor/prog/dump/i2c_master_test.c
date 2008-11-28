/*
 * i2c_master_test.c
 *
 * Test Driver for OMAP2 McBSP driver
 *
 * Copyright (C) 2006 Texas Instruments, Inc.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Author : Chandra Shekhar
 * Date   : 09 jan 2008
 */


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "i2cbusses.h"
#include "i2c-dev.h"
#include "version.h"
#include <fcntl.h>

#define DEFAULT_I2C_BUS      "/dev/i2c-1"

void help(void)
{
	fprintf(stderr, "Syntax: i2c_master_test  slave_addr \
					start_data no_of_bytes( < 256) ");

}

int main(int argc, char *argv[])
{

	struct i2c_rdwr_ioctl_data msg_rdwr;
	struct i2c_msg	i2cmsg;
	int i, fd, x;
	char _buf[256];

	if (argc < 4) {
		fprintf(stderr, " Less  arguments than required \n");
		help();
		exit(0);
	}
	if (atoi(argv[3]) > 256) {
		fprintf(stderr, " maximum data length 256\n");
		help();
		exit(0);
	}

	char *dn = DEFAULT_I2C_BUS;
	if ((fd = open(dn, O_RDWR)) < 0) {
		fprintf(stderr, "Could not open i2c at %s\n", dn);
		perror(dn);
		exit(1);
	}

	for (i = 0; i <= (atoi(argv[3])-1); i++)
		_buf[i] = atoi(argv[2]) + i;

	msg_rdwr.msgs = &i2cmsg;
	msg_rdwr.nmsgs = 1;
	if (atoi(argv[1]) == 1)
		i2cmsg.addr  = 0x68;
	else if (atoi(argv[1]) == 2)
		i2cmsg.addr  = 0x69;
	else if (atoi(argv[1]) == 3)
		i2cmsg.addr  = 0x63;

	i2cmsg.flags &= ~I2C_M_RD;

	i2cmsg.len   = atoi(argv[3]);
	i2cmsg.buf   = _buf;

	if (( x = ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0) {
		perror("ioctl()");
		fprintf(stderr, "ioctl returned %d\n", x);
		return -1;
	}
	exit(0);
}
