#include <stdio.h>
#include <fcntl.h>

int  main(int argc, char *argv[])
{

	printf("Received node: %s\n", argv[1]);
	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		printf("Received node cannot be opened!\n");
		return 1;
	}
	return 0;
}
