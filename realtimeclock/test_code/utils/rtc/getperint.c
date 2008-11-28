
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

	int i, fd, retval, irqcount = 0;
	unsigned long data;
	int choice = 0, count;
	char *choice_data[4] = {"second", "minute", "hour", "day"};

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test \n\n");

	fprintf(stderr, "Enter no. of updates:");
	scanf("%d",&count);
	if ( count < 0 ){
		fprintf(stderr, "Invalid number\n");
		_exit(0);
	}	
	fprintf(stderr,
		"Counting %d update (1/%s) interrupts from reading /dev/rtc0\n",count,choice_data[choice]);
	fflush(stderr);
	/* Turn on update interrupts (one per second) */
	fprintf(stderr, " Turn on update interrupts (one per %s)\n", choice_data[choice]);
	for (i = 1; i <= count; i++) {
		/* This read will block */
		retval = read(fd, &data, sizeof (unsigned long));
		if (retval == -1) {
			perror("read");
			_exit(errno);
		}
		fprintf(stderr, " %d", i);
		fflush(stderr);
		irqcount++;
	}

	fprintf(stderr, "\nAgain, from using select(2) on /dev/rtc0:\n");
	fflush(stderr);
	for (i = 1; i <= count; i++) {
		struct timeval tv = { 5, 0 };	/* 5 second timeout on select */
		fd_set readfds;

		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		/* The select will wait until an RTC interrupt happens. */
		retval = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (retval == -1) {
			perror("select");
			_exit(errno);
		}
		/* This read won't block unlike the select-less case above. */
		retval = read(fd, &data, sizeof (unsigned long));
		if (retval == -1) {
			perror("read");
			_exit(errno);
		}
		fprintf(stderr, " %d", i);
		fflush(stderr);
		irqcount++;
	}

	close(fd);
	return 0;

} /* end main */
