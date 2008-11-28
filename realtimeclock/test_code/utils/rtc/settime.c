#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int get_timedate(struct rtc_time *rtc_tm);
int show_menu(void);
void show_choice(void);

int main(void)
{

	int fd, retval;
	struct rtc_time rtc_tm;
	int choice;

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
		_exit(errno);
	}
	/* Read the TWL4030-RTC time/date */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr,
		"\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	show_choice();

	scanf("%d", &choice);
	if (choice) {
		rtc_tm.tm_sec = 45;	/* second */
		rtc_tm.tm_min = 59;	/* minute */
		rtc_tm.tm_hour = 23;	/* hour */
		rtc_tm.tm_mday = 31;	/* day of the month */
		rtc_tm.tm_mon = 11;	/* month Jan=0, Feb=1 …etc */
		rtc_tm.tm_year = 104;	/* = year - epoch */
		printf("\nUsing default date and time %d-%d-%d, %02d:%02d:%02d",
		       rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		       rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	} else if (choice == 0) {
		get_timedate(&rtc_tm);
	}

	fprintf(stdout,
		"\nSetting New date and time to %d-%d-%d, %02d:%02d:%02d\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);
	if (retval == -1) {
		printf("...failed\n");
		perror("ioctl");
		show_menu();
		_exit(errno);
	} else {
		printf(".....successful\n");
	}

	/* Read the RTC time/date */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stdout,
		"\n\nThe New RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	printf("\n");
	close(fd);

	return 0;
}

int get_timedate(struct rtc_time *rtc_tm)
{
	char date_str[7] = { 0 };
	char time_str[7] = { 0 };
	int num;
	unsigned char data[6] = { 0 };
	int i;

	show_menu();
	printf("Enter Date:");
	scanf("%s", date_str);
	printf("Enter Time:");
	scanf("%s", time_str);
	num = atoi(date_str);
	for (i = 0; i < 3; i++) {
		data[i] = num % 10;
		num /= 10;
		data[i] += (num % 10) * 10;
		num /= 10;
		//   printf ("data[%d] : %d\n", i, data[i]);
	}
	num = atoi(time_str);
	for (; i < 6; i++) {
		data[i] = num % 10;
		num /= 10;
		data[i] += (num % 10) * 10;
		num /= 10;
		//     printf ("data[%d] : %d\n", i, data[i]);
	}
	rtc_tm->tm_mday = data[1];
	rtc_tm->tm_mon = data[2] - 1;
	rtc_tm->tm_year = data[0] + 100;
	rtc_tm->tm_hour = data[5];
	rtc_tm->tm_min = data[4];
	rtc_tm->tm_sec = data[3];
	return 0;

}

int show_menu(void)
{
	int i = 0;
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	printf("*\tSet Date Demo\t");
	printf("\n");
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	printf("*\tUsage : Date format MMDDYY\n");
	printf("*\tUsage : Time format HHMMSS\n");
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	return 0;
}

void show_choice(void)
{
	printf("Set date and time options:\n");
	printf("Enter 0 -> to accept date and time from user\n");
	printf("Enter 1 -> to use default date and time\n");
	printf("Choice:");
}
