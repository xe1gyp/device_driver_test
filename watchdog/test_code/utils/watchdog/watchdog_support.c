#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/watchdog.h>

int main(int argc, const char *argv[])
{

	int return_value;
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

	return_value = ioctl(fd, WDIOC_GETSUPPORT, &wd_test);

	if (!return_value) {
		printf("Watchdog Support\n");
		printf("Identity: %s\n", wd_test.identity);
		printf("Firmware Version: %d\n", wd_test.firmware_version);
		printf("Options: 0x%x\n", wd_test.options);
	} else {
		printf("ioctl WDIOC_GETSUPPORT failed!\n");
		return 1;
	}

	close(fd);
	return return_value;
}
