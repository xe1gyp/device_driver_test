#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/watchdog.h>
#include <pthread.h>
#include <sched.h>

void thread_read(int fd);
#define MAXIMUM_TIME 1000000
#define NUMBER_OF_THREADS 2
int cpunum;
int sleep_time = 1;

int main(int argc, const char *argv[])
{

	int return_value;
	int timeout_value = 5;
	struct watchdog_info wd_test;
	int i;
	pthread_t thread[NUMBER_OF_THREADS];

	if (argc == 3)
		sscanf(argv[argc-1], "%d", &sleep_time);
	else
		sleep_time = 4;

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

	return_value = ioctl(fd, WDIOC_SETTIMEOUT, &timeout_value);

	if (!return_value) {
		printf("Watchdog new timeout value is %d seconds\n", \
			timeout_value);
	} else {
		printf("ioctl WDIOC_SETTIMEOUT failed\n");
		return 1;
	}

	return_value = ioctl(fd, WDIOC_GETTIMEOUT, &timeout_value);

	if (!return_value) {
		printf("Watchdog timeout value is %d seconds\n", timeout_value);
	} else {
		printf("ioctl WDIOC_GETTIMEOUT failed!\n");
		return 1;
	}

	for (i = 0; i < NUMBER_OF_THREADS; i++) {
		if (pthread_create(&thread[i], NULL,
			(void *) thread_read, (int *) fd) != 0) {
			printf("pthread_create() error!");
			exit(1);
	   }
	}

	for (i = 0; i < NUMBER_OF_THREADS; i++)
		pthread_join(thread[i], NULL);

	close(fd);
	return return_value;
}


void thread_read(int fd)
{
	int return_value;
	cpu_set_t cpuset;
	pthread_attr_t attr;
	int result_aff;
	int timeout_value = 5;

	if (cpunum == 0)
		cpunum = 1;
	else
		cpunum = 0;

	pthread_attr_init(&attr);

	fflush(stdout);

	CPU_ZERO(&cpuset);
	CPU_SET(cpunum, &cpuset);
	result_aff = pthread_attr_setaffinity_np
			(&attr, sizeof(cpuset), &cpuset);
	if (result_aff != 0)
		fprintf(stdout,
			"Error setting the affinity...%d \n", result_aff);

	result_aff = pthread_attr_getaffinity_np
			(&attr, sizeof(cpuset), &cpuset);
	if (result_aff != 0)
		fprintf(stdout,
			"Error setting the affinity...%d \n", result_aff);

	fprintf(stdout, "cpuset...%d \n", cpuset);
	fprintf(stdout, "Thread id[%lu]  Starting...\n", pthread_self());

	while (1) {
		sleep(sleep_time);
		return_value = ioctl(fd, WDIOC_KEEPALIVE, &timeout_value);
		if (return_value)
			printf("ioctl WDIOC_KEEPALIVE failed");
		else
			fprintf(stdout, "Thread id[%lu]  Running...\n",\
					pthread_self());
	}
	fprintf(stdout, "Thread id[%lu]  Finished...\n", pthread_self());
}
