
/*
 *	Real Time Clock Driver Test/Example Program
 *
 *	Compile with:
 *	   arm-linux-gcc -s -Wall -Wstrict-prototypes rtctest.c -o rtctest
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
main(void)
{

	int fd, retval;
	int choice = 0;

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}

	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test \n\n");

	fprintf(stderr, "Enter the Periodic IRQ update rate:\n");
	fprintf(stderr, "Allowed Values : \n");
	fprintf(stderr, "1 - updates every second\n");
	fprintf(stderr, "2 - updates every minute\n");
	fprintf(stderr, "4 - updates every hour\n");
	fprintf(stderr, "8 - updates every day\n");
	fprintf(stderr, "Enter your choice:");
	scanf("%d",&choice);
	if ( choice < 1 || choice > 8 ){
		fprintf(stderr, "Invalid Value!!!\n");
		_exit(1);
	}	
	

	retval = ioctl(fd,RTC_IRQP_SET, choice);
	if ( retval == -1 ){
		perror("RTC_IRQP_SET, ioctl");
		_exit(errno);
	}	
	retval = ioctl(fd, RTC_UIE_ON, 0);
	if (retval == -1) {
		perror("RTC_UIE_ON, ioctl");
		_exit(errno);
	}

	printf("Periodic update interrupts are enabled successfully.\n");
	printf("Use ./getperint1 or ./getperint2 prograrm to get interrupt information from TWL4030 RTC module\n");
	printf("Use perintoff to turn off periodic update interrupts\n");
		close(fd);
	return 0;

} /* end main */
