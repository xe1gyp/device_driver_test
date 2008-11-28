
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

	int fd, retval ;

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test \n\n");


	/* Turn off update interrupts */
	retval = ioctl(fd, RTC_UIE_OFF, 0);
	if (retval == -1) {
		perror("ioctl");
		_exit(errno);
	}

	printf("Periodic interrupts are turned off successfully.\n");
	close(fd);
	return 0;

} /* end main */
