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

	#define MAX_LIMIT_INTERACTIONS 10

	int bytes;
	int fd = open(argv[1], O_RDONLY);
	int counter = 0;

	if (argc < 2) {
		printf("Usage: testsfh /dev/input/eventX\n");
		printf("Where X = input device number\n");
		return 1;
	}

	if (fd == -1) {
		printf("Received node cannot be opened!\n");
		return 1;
	}

	printf("\nProgram will wait for %d interactions to finish\n\n",
					MAX_LIMIT_INTERACTIONS);
	fflush(stdout);
	while (counter < MAX_LIMIT_INTERACTIONS) {
		bytes = read(fd, &keyinfo, sizeof(struct input_event));
		if (bytes && keyinfo.type == 0x03) {
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
