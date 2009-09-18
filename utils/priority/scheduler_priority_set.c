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
	int priority;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s [pid] [priority]\n", argv[0]);
		return -1;
	}

	pid=atol(argv[1]);
	priority=atol(argv[2]);

	ret = setpriority(which, pid, priority);
	if (ret != 0) {
		return 1;
        }

	return 0;
}

