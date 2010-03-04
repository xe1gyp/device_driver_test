#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>

void usage()
{
	printf("version <input_subsystem>\n");
}

void print_name(int fd)
{
	char name[256] = "Unknown";

	if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0)
		perror("evdev ioctl");

	printf("Device name: %s\n", name);
}

int main(int argc, char *argv[])
{
	if (argc <= 1) {
		usage();
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);

	if (fd == -1) {
		printf("Could not open %s\n", argv[1]);
		return 1;
	}

	print_name(fd);

	close(fd);
	return 0;
}




