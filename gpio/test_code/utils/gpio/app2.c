#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
int which = PRIO_PROCESS;
id_t pid;
int retn;
int priority=-20;
int gpio =4;
int loop=10,j;

int  main(void)
{
        int ret;

	pid = getpid();
    	retn = setpriority(which, pid, priority);

	printf("App2 entered in UserSpace\n");
	fflush(stdout);
	int fd = open("/dev/gpiotest", O_RDWR);

        if (fd < 0) {
                printf("failed to open /dev/gpiotest\n");
                printf("Did you mknod /dev/gpiotest\n");
		fflush(stdout);
                exit(-1);
        }
	for(j=0;j<loop;j++)
	{
        	ret = write(fd, &gpio, sizeof(gpio));
        	if (ret != sizeof(gpio))
                	printf("App2 write failed. %d at Iteratin %d \n", ret,j);
			fflush(stdout);
		printf("App2 Completed Iteration -- %d\n",j);
		fflush(stdout);
	}
        return(0);

}

