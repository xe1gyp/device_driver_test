#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

int main(int argc, char * argv[])
{
	int ret;
	int which = PRIO_PROCESS;
	id_t pid;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s [pid]\n", argv[0]);
		return -1;
	}

	pid=atol(argv[1]);

	ret = getpriority(which, pid);
	printf("%d\n", ret);

	return 0;
}
