#include <stdio.h>
#include <fcntl.h>

/*
 * To run this program we have to choose Event Interface under
 * Input Device Support when in menuconfig
 *
 */

int main(int argc, char *argv[])
{
	struct input_event {
		struct timeval time;
		unsigned short type;
		unsigned short code;
		unsigned int value;
	} keyinfo;

	int bytes;
	int fd;
	int counter = 0;
	int iterations = 0;

	if (argc < 3) {
		printf("Usage: testkeypad /dev/input/eventX <iterations>\n");
		printf("Where X = input device number\n");
		return 1;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		printf("Received node cannot be opened!\n");
		return 1;
	}

	iterations = atoi(argv[2]);
	printf("Number of iterations %d\n", iterations);
	if (iterations < 1) {
		printf("number of iterations shall be > 0\n");
		return 1;
	}

	printf("\nProgram will wait for %d interactions to finish\n\n",
					iterations);
	fflush(stdout);
	while (counter < iterations) {
		bytes = read(fd, &keyinfo, sizeof(struct input_event));
		if (bytes && keyinfo.type == 0x01) {
			printf("sequence= %d > time=%ld | sec %ld "
						"microsec | code=%d | "
						"value=%d\n",
						counter,
						keyinfo.time.tv_sec,
						keyinfo.time.tv_usec,
						keyinfo.code,
						keyinfo.value);
			fflush(stdout);
			counter++;
		}
	}
	return 0;
}
