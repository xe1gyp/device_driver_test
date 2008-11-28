#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>


#include "lib.h"

static int usage(void)
{
	printf("Usage: setlink <vid>\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int vid, fd, ret;
	int link;	

	if (argc < 2)
		return usage();

	vid = atoi(argv[1]);
	if ((vid != 1) && (vid != 2)){
		printf("vid has to be 1 or 2!\n");
		return usage();
	}

	fd = open ((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDONLY) ;
	if (fd <= 0) {
		printf("Could not open %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
		return -1;
	}
	else
		printf("openned %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);

	ret = ioctl (fd, VIDIOC_G_OMAP2_LINK, &link);
	if (ret < 0) {
		perror ("VIDIOC_G_OMAP2_LINK");
		return 0;
	}
	printf("link status: V%d is %s linked to another layer\n", vid,
		(link == 1)? "": "not");

	link = 1;
	ret = ioctl (fd, VIDIOC_S_OMAP2_LINK, &link);
	if (ret < 0) {
		perror ("VIDIOC_S_OMAP2_LINK");
		return 0;
	}
	printf("linked!\n");

	if (ioctl(fd, VIDIOC_STREAMON, &link) == -1){
		perror("VIDIOC_STREAMON");
		return;
	}

	sleep(30);

	if (ioctl(fd, VIDIOC_STREAMOFF, &link) == -1){
		perror("VIDIOC_STREAMOFF");
		return;
	}
	link = 0;
	ret = ioctl (fd, VIDIOC_S_OMAP2_LINK, &link);
	if (ret < 0) {
		perror ("VIDIOC_S_OMAP2_LINK");
		return 0;
	}
	printf("unlinked!\n");


	close(fd) ;
}
