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
	printf("Usage: mirroring <vid> <enable[1]/disable[0]>\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int vid, fd, ret;
	int state;	

	if (argc < 3)
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

        state = atoi(argv[2]); 

	ret = ioctl (fd,VIDIOC_S_OMAP2_MIRROR, &state);
	if (ret < 0) {
		perror ("VIDIOC_S_OMAP2_MIRROR");
		return 0;
	}
	printf("Mirroring set to %d \n",state);
	close(fd) ;
}
