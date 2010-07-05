#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/watchdog.h>

int main(int argc, const char *argv[])
{

	int return_value;
	int timeout_value;
	struct watchdog_info wd_test;

	if (argc < 3) {
		printf("Please specify <devfs interface> and <timeout>" \
			" for watchdog!\n");
		printf("<timeout> in seconds\n");
		printf("E.g. %s /dev/watchdog 2\n", argv[0]);
		exit(0);
	}

	sscanf(argv[2], "%d", &timeout_value);
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

	close(fd);
	return return_value;

}
