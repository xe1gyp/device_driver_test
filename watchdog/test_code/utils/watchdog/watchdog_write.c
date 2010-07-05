#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/watchdog.h>

int main(int argc, const char *argv[])
{

	int return_value;
	int timeout_value = 2;
	struct watchdog_info wd_test;

	if (argc < 2) {
		printf("Please specify <devfs interface> for watchdog!\n");
		printf("E.g. %s /dev/watchdog\n", argv[0]);
		exit(0);
	}

	int fd = open(argv[1], O_WRONLY);

	if (fd == -1) {
		perror("Watchdog device interface is not available!\n");
		return 1;
	}

	return_value = ioctl(fd, WDIOC_SETTIMEOUT, &timeout_value);

	if (!return_value) {
		printf("Watchdog new timeout value is %d seconds\n", \
			timeout_value);
	} else {
		printf("ioctl WDIOC_SETTIMEOUT failed\n");
		return 1;
	}

	return_value = ioctl(fd, WDIOC_GETTIMEOUT, &timeout_value);

	if (!return_value) {
		printf("Watchdog timeout value is %d seconds\n", timeout_value);
	} else {
		printf("ioctl WDIOC_GETTIMEOUT failed!\n");
		return 1;
	}

	while (1) {
		write(fd, "\0", 1);
		sleep(1);
	}

	close(fd);
	return return_value;

}
