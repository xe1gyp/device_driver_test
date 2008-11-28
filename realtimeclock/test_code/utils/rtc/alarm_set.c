
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
void show_usage(void);
int validate_data(int fd, struct rtc_time *rtc_almtm);
int get_timedate(struct rtc_time *rtc_tm);
int show_menu(void);

int main(int argc, char *argv[])
{

	int fd, retval; 
	struct rtc_time rtc_tm, rtc_almtm;
	int choice, secs = 0;

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test Example.\n\n");
	show_usage();
	scanf("%d", &choice);

	if (choice == 0 || choice == 1) {
		/* Read the RTC time/date */
		if ( choice == 1 ){
			printf("Enter alarm time in seconds :");
			scanf("%d",&secs);
			if ( secs <= 0){
				printf("Invalid alarm time\n");
				_exit(-1);
			}	
		}	
			
		retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
		if (retval == -1) {
			perror("ioctl");
			_exit(errno);
		}

		fprintf(stderr,
			"\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
			rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
			rtc_tm.tm_sec);

		/* Set the alarm to 5 sec in the future, and check for rollover */
		if ( choice == 0 )
			rtc_tm.tm_sec += 5;
		else 
			rtc_tm.tm_sec += secs;
		
		while ( rtc_tm.tm_sec >= 60 ){
			rtc_tm.tm_sec -= 60;
			rtc_tm.tm_min++;
		}
		while ( rtc_tm.tm_min >= 60 ){
			rtc_tm.tm_min -= 60;
			rtc_tm.tm_hour++;
		}
		while ( rtc_tm.tm_hour >= 24 ){
			rtc_tm.tm_hour -= 24;
			rtc_tm.tm_mday++;
		}	

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
	} else if (choice == 2) {
		get_timedate(&rtc_almtm);
		if (validate_data(fd, &rtc_almtm) < 0 ){
			printf("Invalid Alarm value\n");
		}
		retval = ioctl(fd, RTC_ALM_SET, &rtc_almtm);
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
		fprintf(stderr,
			"\n\nAlarm date and time now set to %d-%d-%d, %02d:%02d:%02d.\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
			rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
			rtc_tm.tm_sec);

	}
	} else {
		printf("Invalid choice\n");
		_exit(-1);
	}

	/* Enable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_ON, 0);

	if ( choice == 1 )
		printf("Run ./getalarm_event with in %d seconds to receive alarm notification\n", secs);
	
	close(fd);
	return 0;

}				/* end main */

void show_usage(void)
{
	printf("Set alarm time options\n");
	printf("0 -> default alarm time is set to 5 secs from current time\n");
	printf("1 -> Enter alarm time in secs:\n");
	printf
	    ("2 -> Enter alarm date(format : MMDDYY) and time(format : HHMMSS)\n");
	printf("Choice:");
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

int validate_data(int fd, struct rtc_time *rtc_almtm)
{
	struct rtc_time rtc_tm;
	int retval;
	int time_nosecs, almtime_nosecs;
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	fprintf(stderr,
		"\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1,
		rtc_tm.tm_year + 1900, rtc_tm.tm_hour, rtc_tm.tm_min,
		rtc_tm.tm_sec);
	time_nosecs =
	    rtc_tm.tm_sec + rtc_tm.tm_min * 60 + rtc_tm.tm_hour * 24 +
	    365 * rtc_tm.tm_year;
	almtime_nosecs =
	    rtc_almtm->tm_sec + rtc_almtm->tm_min * 60 +
	    rtc_almtm->tm_hour * 24 + 365 * rtc_almtm->tm_year;
	if (almtime_nosecs <= time_nosecs)
		return -1;
	else
		return 0;
}

int show_menu(void)
{
	int i = 0;
	for (i = 0; i++ < 40;)
		printf("*");
	printf("\n");
	printf("*\tSet Alarm Date and Time Demo\t");
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

