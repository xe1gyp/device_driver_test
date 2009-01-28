#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd, i;
	size_t size;
	char *buffer;

	printf("Received node: %s\n", argv[1]);
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		perror("Error! devfs entry is not present!");

	/* Number of random numbers to be generated */
	size = 10;
	buffer = (char *)malloc(size);
	read(fd, buffer, size);
	for (i = 0; i < size; i++)
		printf("The data is %x\n", buffer[i]);
}
