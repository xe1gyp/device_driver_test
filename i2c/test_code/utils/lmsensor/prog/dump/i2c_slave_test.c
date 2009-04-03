/*
 * i2c_slave_test.c
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

#define LEN 257

#define DEFAULT_I2C_BUS		"/dev/i2c-3"
void help(void)
{
	fprintf(stderr, "Syntax: i2c_slave_test  slave_addr ");

}


int main(int argc, char *argv[])
{

	struct i2c_rdwr_ioctl_data  msg_rdwr;
	struct i2c_msg  i2cmsg;
	int i, fd, x, num;
	char _buf[257];

	char *dn = DEFAULT_I2C_BUS;
	if ((fd = open(dn, O_RDWR)) < 0) {
		fprintf(stderr, "Could not  open i2c at %s\n", dn);
		perror(dn);
		exit(1);
	}
	if (argc < 2) {
		help();
		return(0);
	}

	msg_rdwr.msgs = &i2cmsg;
	msg_rdwr.nmsgs = 1;
	if (atoi(argv[1]) == 1)
		i2cmsg.addr  = 0x8068;
	else if (atoi(argv[1]) == 2)
		i2cmsg.addr  = 0x8069;
	else if (atoi(argv[1]) == 3)
		i2cmsg.addr  = 0x8063;
	i2cmsg.flags |= I2C_M_HS;
	i2cmsg.flags |= I2C_M_RD;

	i2cmsg.len   = LEN;
	i2cmsg.buf   = _buf;

	if ((x = ioctl(fd, I2C_RDWR, &msg_rdwr)) < 0) {
		perror("ioctl()");
		fprintf(stderr, "ioctl returned %d\n", x);
		return -1;
	}
	num = _buf[0];

	for (i = 0; i <= num; i++) /* copy buf[0..n] -> _buf[1..n+1] */
	printf(" data = 0x%d \n", _buf[i]);

	return (0);

}
