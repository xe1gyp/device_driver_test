#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>

void print_message_function( void );
int count = 0;

main(int argc, char *argv[])
{
	int priority, ret;
	int which = PRIO_PROCESS;
	int pid = getpid();

	if (argc != 3) {
		printf("Tool less arguments provided\n"\
			"Give args as: proc [priority] [count]\n");
		return 1;
	}
	else {
		priority = atoi(argv[1]);
		ret = setpriority(which, pid, priority);
		count = atoi(argv[2]);
	}

	print_message_function();

	printf("Proc2 is returning\n");
	return 0;
}

void print_message_function( void )
{
	int loop;

	for (loop = 0; loop < count; loop++)
	{
		if (system(NULL)) {
			system("cp /tmp/testfile.txt file2");
		}
		printf("Copied file2 for %d time\n", loop+1);
	}
}
