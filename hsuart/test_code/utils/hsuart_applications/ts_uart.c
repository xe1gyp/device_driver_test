/*
 * HS-UART TEST PROGRAM
 *
 * Driver location: linux/drivers/serial/omap-serial.c
 * This application is meant for testing the UART
 * driver on OMAP Platform.
 *
 * Copyright (C) 2008-2009 Texas Instruments, Inc.
 * Govindraj R <govindraj.raja@ti.com>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * History:
 */

#include "common.h"

#define TIOCMSET 0x5418
#define TIOCM_LOOP 0x8000

int main(int argc, char **argv)
{
	int i = 0, rd = 0;
	fd_set readFds;
	int retval;
	struct timeval respTime;
	unsigned char input_buf[bufsize], output_buf[bufsize];
	char uartportname[20];
	int size = 0;
	int chk_flag, error = 0;

	unsigned long loopback_val = TIOCM_LOOP;

	char md5_sum1[33];
	char cmd_buf [20];

	/* to handle ctrl +c */
	if (signal(SIGINT, signalHandler) == SIG_ERR) {
		printf("ERROR : Failed to register signal handler\n");
		exit(1);
	}
	if (argc != 5) {
		display_intro();
		exit(1);
	}

	chk_flag = sscanf(argv[3], "%li", &ut.baudrate);
	if (chk_flag != 1)
		error = 1;
	chk_flag = sscanf(argv[4], "%i", &ut.flow_cntrl);
	if (chk_flag != 1)
		error = 1;

	if (ut.flow_cntrl < 0 || ut.flow_cntrl > 2)
		error = 1;
	if (error) {
		printf("\n [%s] or [%s] : Invalid command line argument \n",
							argv[3], argv[4]);
		display_intro();
		exit(1);
	}
	sprintf(uartportname, "/dev/%s", argv[2]);
	uartportname[strlen(argv[2]) + 5] = '\0';
	printf("\n Opening %s \n", uartportname);
	ut.fd = open(uartportname, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

	if (ut.fd == -1) {
		printf("Failed to open the port %s, returncode=%d\n",
			uartportname, ut.fd);
		exit(1);
	} else {
		fcntl(ut.fd, F_SETFL, 0);
	}
	printf("\n Existing port baudrate= %d", getbaud(ut.fd));
	/* save current port settings */
	tcgetattr(ut.fd, &oldtio);
	initport(ut.fd, ut.baudrate, ut.flow_cntrl);
	printf("\n Configured port for baudrate= %d", getbaud(ut.fd));

	sscanf(argv[1], "%c", &tx_rx);

	error = ioctl(ut.fd, TIOCMBIS, &loopback_val);
	if (error) {
		printf("\n ERROR in setting UART_LOOPBACK: \n \
			 ioctl error ret = %d \n", error);
		exit(1);
	} else {
		printf("\n Sucessfull in setting UART in LOOPBACK MODE \n");
	}

	switch (tx_rx) {
	case 'r':
		/* open(const char *pathname, int flags, mode_t mode); */
		sprintf(tx_rx_filename, "uart_rx_file_%s", argv[2]);
		fd2 = open(tx_rx_filename, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXO);

		if (fd2 == -1) {
			printf("\n Failure in opening %s \n", tx_rx_filename);
			close(ut.fd);
			exit(1);
		}
		read_flag = 0;
		while (1) {
			FD_ZERO(&readFds);
			FD_SET(ut.fd, &readFds);
			respTime.tv_sec = 30;
			respTime.tv_usec = 0;

			/*
			 * Sleep for command timeout
			 * and expect the response to be ready.
			 */
			retval = select(FD_SETSIZE, &readFds,
					NULL, NULL, &respTime);

			if (retval == ERROR)
				printf("\n select: error :: %d\n", retval);

			/* Read data, abort if read fails. */
			if (FD_ISSET(ut.fd, &readFds) != 0)  {
			/*	fcntl(ut.fd, F_SETFL, FNDELAY);	*/
			/*	printf("\n Entering readport \n"); */
				if (read_flag == 0) {
					gettimeofday(&ut.start_time, NULL);
					read_flag = 1;
				}
				rd = readport(&ut.fd, output_buf);
				if (ERROR == rd) {
					printf("Read Port failed\n");
					close_port();
				}
			}
			if (read_flag == 0) {
				if (unlink(tx_rx_filename) == -1)
					printf("\n Failed to delete the \
						file %s \n", tx_rx_filename);
				printf("\n Waited for 30 seconds no data was \
						available to read, exiting \n");
				close_port();
			}
			if (rd == 0)
				break;
			size += rd;
			i = write(fd2, &output_buf, rd);
			memset(output_buf, 0, bufsize);
			/* printf("\nport returned %d bytes Written %d \
				bytes to output file",rd,i);
			 */
		}
		gettimeofday(&ut.end_time, NULL);
		printf("\n Read %d bytes from port \n", size);

		sprintf(cmd_buf, "md5sum %s ", tx_rx_filename);
		md5_fd = popen(cmd_buf, "r");
		if (!ferror(md5_fd)) {
			/* md5sum returns 32 bit checksum value */
			fgets(md5_sum1, 32, md5_fd);
			/* Append the read checksum value with null string */
			md5_sum1[32] = '\0';
		} else {
			printf("\n Check sum generation for %s failed \n", tx_rx_filename);
			break;
		}
		printf("\n Checksum generated for %s = \n\t %s \n",
					tx_rx_filename, md5_sum1);

		FD_CLR(ut.fd, &readFds);
		break;
	case 's':
		/* Create an sample 1MB file to send */
		fd1 = create_sample_send_file(argv[2]);

		if (fd1 == ERROR) {
			printf("\n cannot create %s sample file for tx \n", tx_rx_filename);
			close(ut.fd);
			exit(1);
		}
		gettimeofday(&ut.start_time, NULL);
		while (1) {
			rd = read(fd1, &input_buf, bufsize);
			if (rd == 0)
			break;
			size += rd;
			/* printf("\n Read from input file %d bytes \n",rd); */
			fcntl(ut.fd, F_SETFL, 0);
			if (!writeport(&ut.fd, input_buf, rd)) {
				printf("\n Writing to port failed\n");
				close_port();
			}
			if (ERROR == tcdrain(ut.fd))
				printf("\n tcdrain failure \n");
			memset(input_buf, 0, bufsize);
		}
		gettimeofday(&ut.end_time, NULL);
		printf("\n Written %d bytes from port \n", size);
		/* Wait for 3 seconds for Transmition to complete before
		 * sending the Break sequence */
		sleep(3);
		/* for(i=0;i<2;i++) */
		if (tcsendbreak(ut.fd, 5) < 0)
			printf("\n Sending break sequence fialed use \
				 ctrl + c to terminate read process\n");

		sprintf(cmd_buf, "md5sum %s", tx_rx_filename);
		md5_fd = popen(cmd_buf, "r");
		if (!ferror(md5_fd)) {
			/* md5sum returns 32 bit checksum value */
			fgets(md5_sum1, 32, md5_fd);
			/* Append the read checksum value
			 * with null string */
			md5_sum1[32] = '\0';
		} else {
			printf("\n Check sum generation for %s \
					failed \n", tx_rx_filename);
			break;
		}
		printf("\n CheckSum generated for %s = \n\t %s \n",
					tx_rx_filename, md5_sum1);
		break;
	default:
		printf("\n Unspecified operation %c:", tx_rx);
		break;
	}

	timersub(&ut.end_time, &ut.start_time, &ut.diff_time);
	if (tx_rx == 'r')
		 ut.diff_time.tv_sec -= 3;
	printf("\n Time taken %08ld sec, %08ld usec\n\n ",
				ut.diff_time.tv_sec, ut.diff_time.tv_usec);
	pclose(md5_fd);
	/* restore the old port settings. */
	close_port();
	return 0;
}
