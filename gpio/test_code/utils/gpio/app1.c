#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
id_t pid;
int ret;

int  main(void)
{
	int gpio=3;
        int ret;
        int loop =10;
	int i;

	int fd = open("/dev/gpiotest", O_RDWR);

        if (fd < 0) {
                printf("failed to open /dev/gpiotest\n");
                printf("Did you mknod /dev/gpiotest \n");
		fflush(stdout);
                exit(-1);
        }

	for (i=0;i<loop;i++)
	{
		ret = write(fd, &gpio, sizeof(gpio));
		if (ret != sizeof(gpio)) 
		{
			printf("App1 write failed %d at iteration %d", ret,i);
			fflush(stdout);
			break;
		}
		printf("App1 Completed Iteration -- %d\n",i);
		fflush(stdout);
	}
	return(0);
}
