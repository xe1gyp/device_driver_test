#include <stdio.h>
#include <fcntl.h>

int main(void)
{
	int fd = open("/dev/input/event1", O_RDONLY);
	if (fd > 0) {
		close(fd);
		return 0;
	}
}
