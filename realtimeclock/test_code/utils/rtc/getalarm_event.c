
/*
 *	Real Time Clock Driver Test/Example Program
 *
 *	Compile with:
 *		gcc -s -Wall -Wstrict-prototypes rtctest.c -o rtctest
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{

	int fd, retval; 
	unsigned long data;
	struct rtc_time rtc_tm; 

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		fprintf(stdout, "Error...!!! /dev/rtc0 not present.");
		return 1;
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test Example.\n\n");
	/* Read the current alarm settings */
	retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}


	fprintf(stderr,
			"\n\nWaiting for alarm at date and time  %d-%d-%d, %02d:%02d:%02d.\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
			rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
			rtc_tm.tm_sec);
		
	fflush(stderr);
	/* This blocks until the alarm ring causes an interrupt */
	retval = read(fd, &data, sizeof (unsigned long));
	if (retval == -1) {
		perror("read");
		_exit(errno);
	}
	fprintf(stdout, " okay. Alarm rang.\n");

	close(fd);
	return 0;

}				/* end main */

