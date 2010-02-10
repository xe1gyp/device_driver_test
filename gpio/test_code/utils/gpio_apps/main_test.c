#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_APP 	2
#define MAX_LENGTH 	32

static pid_t appPidList[MAX_APP] = { -1, -1};
static char appList[MAX_APP][MAX_LENGTH] =
	{
		"/bin/app1",
		"/bin/app2"
	};

int main(void)
{

	int i = 0;
	for (i = 0; i < MAX_APP; i++) {

		appPidList[i] = fork();
		if (appPidList[i] < 0) {
			printf("fork failed for %s", appList[i]);
			fflush(stdout);
			exit(-1);
		} else {
			/* Child Process */
			if (appPidList[i] == 0) {
				printf("Spawned in main %s", appList[i]);
				fflush(stdout);
				if (execlp("/bin/sh", "sh", "-exec", \
						appList[i], (char *)0) < 0) {
					printf("execlp failed for %s", \
							appList[i]);
					fflush(stdout);
					exit(-1);
				} else {
					printf("application %d --%s is "
							"successfully " \
							"launched\n", \
							i, appList[i]);
					fflush(stdout);
				}
			}
		}
		usleep(100);
	}
	fflush(stdout);
	return 0;
}
