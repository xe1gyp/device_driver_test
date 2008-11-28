/*
    i2cdetect.c - a user-space program to scan for I2C devices
    Copyright (C) 1999-2004  Frodo Looijaard <frodol@dds.nl>,
                             Mark D. Studebaker <mdsxyz123@yahoo.com> and
                             Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../dump/i2cbusses.h"
#include "i2c-dev.h"
#include "version.h"

#define MODE_AUTO	0x00
#define MODE_QUICK	0x01
#define MODE_READ	0x02
#define MODE_HS		0x04
#define MODE_TEN	0x08
#define MODE_I2C	0x10

void help(void)
{
	fprintf(stderr,
		"Syntax: i2cdetect  [-y] [-a] [-q|-r] [-h] [-t] I2CBUS [FIRST LAST]\n"
		"        i2cdetect -l\n"
		"        i2cdetect -V\n"
		"  I2CBUS is an integer\n"
		"  With -a, probe all addresses (NOT RECOMMENDED)\n"
		"  With -q, uses only quick write commands for probing (NOT "
		"RECOMMENDED)\n"
		"  With -r, uses only read byte commands for probing (NOT "
		"RECOMMENDED)\n"
		"  With -h, uses High speed mode (NOT RECOMMENDED)\n"
		"  With -t, uses Ten Bit addressing mode (NOT RECOMMENDED)\n"
		"  With -i, uses i2c dev's read and write calls to probe the chips\n"
		"  If provided, FIRST and LAST limit the probing range.\n"
		"  With -l, lists installed busses only\n");
	print_i2c_busses(0);
}

static int write_i2c_bus(int file, int i2c)
{
	int res = 0;
	char data = 0;
	if (!i2c) {
		res = i2c_smbus_write_quick(file, I2C_SMBUS_WRITE);
	} else {
		res = write(file, &data, 1);	/* 0 byte is not supproted */
	}
	return res;
}
static int read_i2c_bus(int file, int i2c)
{
	int res = 0;
	char data;
	if (!i2c) {
		res = i2c_smbus_read_byte(file);
	} else {
		res = read(file, &data, 1);
		if (res >= 0)
			res = data;
	}
	return res;
}

int scan_i2c_bus(int file, int mode, const int first, int last)
{
	int i, j;
	int res;
	int hs = mode & MODE_HS;
	int ten = mode & MODE_TEN;
	int i2c = mode & MODE_I2C;
	int max_addr = 0;
	mode &= ~(MODE_HS | MODE_TEN | MODE_I2C);

	if ((hs) && (ioctl(file, I2C_HIGH_SPEED, 1) < 0)) {
		fprintf(stderr, "Error:Could not set HS mode - %s\n",
			strerror(errno));
		return -1;
	}

	if (hs) {
		printf("HS mode ");
	}

	if ((ten) && (ioctl(file, I2C_TENBIT, 1) < 0)) {
		fprintf(stderr,
			"Error:Could not set Ten Bit addressing mode - %s\n",
			strerror(errno));
		return -1;
	}

	if (ten) {
		max_addr = 1024;
		if (last == 0x7F) {
			last = 0x3FF;	/* max for ten bit mode */
		}
		printf("10 bit mode");
	} else {
		max_addr = 128;
	}
	printf("\n     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

	for (i = 0; i < max_addr; i += 16) {
		printf("%02x: ", i);
		for (j = 0; j < 16; j++) {
			/* Skip unwanted addresses */
			if (i + j < first || i + j > last) {
				printf("   ");
				continue;
			}

			/* Set slave address */
			if (ioctl(file, I2C_SLAVE, i + j) < 0) {
				if (errno == EBUSY) {
					printf("UU ");
					continue;
				} else {
					fprintf(stderr,
						"Error: Could not set "
						"address to 0x%02x: %s\n",
						i + j, strerror(errno));
					return -1;
				}
			}

			/* Probe this address */
			switch (mode) {
			case MODE_QUICK:
				/* This is known to corrupt the Atmel AT24RF08
				   EEPROM */
				res = write_i2c_bus(file, i2c);
				break;
			case MODE_READ:
				/* This is known to lock SMBus on various
				   write-only chips (mainly clock chips) */
				res = read_i2c_bus(file, i2c);
				break;
			default:
				if ((i + j >= 0x30 && i + j <= 0x37)
				    || (i + j >= 0x50 && i + j <= 0x5F))
					res = read_i2c_bus(file, i2c);
				else
					res = write_i2c_bus(file, i2c);
			}

			if (res < 0)
				printf("XX ");
			else
				printf("%02x ", i + j);
		}
		printf("\n");
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char *end;
	int i2cbus, file, res;
	char filename[20];
	long funcs;
	int mode = MODE_AUTO;
	int first = 0x03, last = 0x77;
	int flags = 0;
	int yes = 0, version = 0, list = 0;

	/* handle (optional) flags first */
	while (1 + flags < argc && argv[1 + flags][0] == '-') {
		switch (argv[1 + flags][1]) {
		case 'V':
			version = 1;
			break;
		case 'y':
			yes = 1;
			break;
		case 'l':
			list = 1;
			break;
		case 'r':
			if (mode & MODE_QUICK) {
				fprintf(stderr,
					"Error: Different modes "
					"specified!\n");
				exit(1);
			}
			mode |= MODE_READ;
			break;
		case 'q':
			if (mode & MODE_READ) {
				fprintf(stderr,
					"Error: Different modes "
					"specified!\n");
				exit(1);
			}
			mode |= MODE_QUICK;
			break;
		case 'h':
			mode |= MODE_HS;
			break;
		case 't':
			mode |= MODE_TEN;
			break;
		case 'i':
			mode |= MODE_I2C;
			break;
		case 'a':
			first = 0x00;
			last = 0x7F;
			break;
		default:
			fprintf(stderr,
				"Warning: Unsupported flag "
				"\"-%c\"!\n", argv[1 + flags][1]);
			help();
			exit(1);
		}
		flags++;
	}

	if (version) {
		fprintf(stderr, "i2cdetect version %s\n", LM_VERSION);
		exit(0);
	}

	if (list) {
		print_i2c_busses(1);
		exit(0);
	}

	if (argc < flags + 2) {
		fprintf(stderr, "Error: No i2c-bus specified!\n");
		help();
		exit(1);
	}
	i2cbus = strtol(argv[flags + 1], &end, 0);
	if (*end) {
		fprintf(stderr, "Error: I2CBUS argument not a number!\n");
		help();
		exit(1);
	}
	if ((i2cbus < 0) || (i2cbus > 0xff)) {
		fprintf(stderr,
			"Error: I2CBUS argument out of range " "(0-255)!\n");
		help();
		exit(1);
	}

	/* read address range if present */
	if (argc == flags + 4) {
		int tmp;

		tmp = strtol(argv[flags + 2], &end, 0);
		if (*end) {
			fprintf(stderr,
				"Error: FIRST argment not a " "number!\n");
			help();
			exit(1);
		}
		if (tmp < first || tmp > last) {
			fprintf(stderr,
				"Error: FIRST argument out of range "
				"(0x%02x-0x%02x)!\n", first, last);
			help();
			exit(1);
		}
		first = tmp;

		tmp = strtol(argv[flags + 3], &end, 0);
		if (*end) {
			fprintf(stderr,
				"Error: LAST argment not a " "number!\n");
			help();
			exit(1);
		}
		if (tmp < first || tmp > last) {
			fprintf(stderr,
				"Error: LAST argument out of range "
				"(0x%02x-0x%02x)!\n", first, last);
			help();
			exit(1);
		}
		last = tmp;
	} else if (argc != flags + 2) {
		help();
		exit(1);
	}

	file = open_i2c_dev(i2cbus, filename);
	if (file < 0) {
		exit(1);
	}

	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr,
			"Error: Could not get the adapter "
			"functionality matrix: %s\n", strerror(errno));
		close(file);
		exit(1);
	}
	if (!(mode & MODE_I2C) && (!(mode & MODE_READ)
				   && !(funcs & I2C_FUNC_SMBUS_QUICK))) {
		fprintf(stderr,
			"Error: Can't use SMBus Quick Write command "
			"on this bus (ISA bus?)\n");
		close(file);
		exit(1);
	}
	if (!(mode & MODE_QUICK)
	    && !(funcs & I2C_FUNC_SMBUS_READ_BYTE)) {
		fprintf(stderr,
			"Error: Can't use SMBus Read Byte command "
			"on this bus (ISA bus?)\n");
		close(file);
		exit(1);
	}
	if ((mode & MODE_HS) && (!(funcs & I2C_FUNC_HIGH_SPEED))) {
		fprintf(stderr,
			"Error: Can't use High Speed mode on this bus\n");
		close(file);
		exit(1);
	}

	if (!yes) {
		char s[2];

		fprintf(stderr,
			"WARNING! This program can confuse your I2C "
			"bus, cause data loss and worse!\n");

		fprintf(stderr, "I will probe file %s%s.\n",
			filename,
			((mode & MODE_READ)
			 && (mode & MODE_I2C)) ? " using I2C Read cmds" : (mode
									   &
									   MODE_READ)
			? " using read commands" : ((mode & MODE_QUICK)
						    && (mode & MODE_I2C)) ?
			" using I2C  write commands" : (mode & MODE_QUICK) ?
			" using quick write commands" : "");
		fprintf(stderr, "I will probe address range 0x%02x-0x%02x.\n",
			first, last);

		fprintf(stderr, "Continue? [Y/n] ");
		fflush(stderr);
		fgets(s, 2, stdin);
		if (s[0] != '\n' && s[0] != 'y' && s[0] != 'Y') {
			fprintf(stderr, "Aborting on user request.\n");
			exit(0);
		}
	}

	res = scan_i2c_bus(file, mode, first, last);

	close(file);

	exit(res ? 1 : 0);
}
