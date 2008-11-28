
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
	unsigned long tmp, data;
	int choice = 0, count;
	char *choice_data[4] = {"second", "minute", "hour", "day"};

	fd = open("/dev/rtc0", O_RDONLY);
	if (fd == -1) {
		perror("Error...!!! /dev/rtc0 not present.");
	}
	fprintf(stderr, "\n\t\t\tTWL4030 RTC Driver Test \n\n");

	fprintf(stderr, "Enter no. of updates:");
	scanf("%d",&count);
        //count = 1000;
	if ( count < 0 ){
		fprintf(stderr, "Invalid number\n");
		_exit(0);
	}	
	fprintf(stderr,
		"Counting %d update (1/%s) interrupts from reading /dev/rtc0\n",count,choice_data[choice]);
	fflush(stderr);
	/* Read periodic IRQ rate */
	retval = ioctl(fd, RTC_IRQP_READ, &tmp);
	
	if (retval == -1) {
		printf(" not able to read periodic irq rate\n");
		perror("ioctl");
		_exit(errno);
	}
	
	switch (tmp) {
	case 1:
		tmp = 0;
		break;
	case 2:
		tmp = 1;
		break;
	case 4:
		tmp = 2;
		break;
	case 8:
		tmp = 3;
		break;
	}

	fprintf(stderr, " Update interrupts (one per %s)\n", choice_data[tmp]);
	printf("Using read system call.........\n");
	for (i = 1; i <= count; i++) {
		/* This read will block */
		retval = read(fd, &data, sizeof (unsigned long));
		if (retval == -1) {
			perror("read");
			_exit(errno);
		}
		fprintf(stdout, " %d", i);
		fflush(stdout);
		irqcount++;
	}

	printf("\n");
	close(fd);
	return 0;

} /* end main */
