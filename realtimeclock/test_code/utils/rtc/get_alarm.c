
/*
 *	Real Time Clock Driver Test/Example Program
 *
 *	Compile with:
 *		gcc -s -Wall -Wstrict-prototypes rtctest.c -o rtctest
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

int
main(int argc, char *argv[])
{

	int fd, retval, irqcount = 0;
	unsigned long data;
	struct rtc_time rtc_tm;
	int alarm_time;

	fd = open("/dev/misc/omap-twl4030rtc", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/misc/omap-twl4030rtc not present.");
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test Example.\n\n");

	/* Read the RTC time/date */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr,
		"\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	/* Set the alarm to 5 sec in the future, and check for rollover */
	rtc_tm.tm_sec += 60;
	if (rtc_tm.tm_sec >= 60) {
		rtc_tm.tm_sec %= 60;
		rtc_tm.tm_min++;
	}
	if (rtc_tm.tm_min == 60) {
		rtc_tm.tm_min = 0;
		rtc_tm.tm_hour++;
	}
	if (rtc_tm.tm_hour == 24)
		rtc_tm.tm_hour = 0;

	retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	printf("alarm setting done...\n");

	/* Read the current alarm settings */
	retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, "Alarm time now set to %02d:%02d:%02d.\n",
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	/* Enable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_ON, 0);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr, "Waiting 5 seconds for alarm...");
	fflush(stderr);
	/* This blocks until the alarm ring causes an interrupt */
	//retval = read(fd, &data, sizeof (unsigned long));
	if (retval == -1) {
		perror("read");
		_exit(errno);
	}
	irqcount++;
	fprintf(stderr, " okay. Alarm rang.\n");

	/* Disable alarm interrupts */
//	retval = ioctl(fd, RTC_AIE_OFF, 0);

	if (retval == -1) {
		printf(" alarm interrupts not disabled\n");
		perror("ioctl");
		_exit(errno);
	}

	close(fd);
	return 0;

}/* end main */

