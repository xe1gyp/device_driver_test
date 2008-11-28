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
	printf("Usage: setwin <vid> <left> <top> <width> <height>\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int vid, fd, ret;	
	struct v4l2_format format;

	if (argc < 6)
		return usage();

	vid = atoi(argv[1]);
	if ((vid != 1) && (vid != 2)){
		printf("vid has to be 1 or 2!\n");
		return usage();
	}

	format.fmt.win.w.left=atoi(argv[2]);
	format.fmt.win.w.top=atoi(argv[3]);
	format.fmt.win.w.width=atoi(argv[4]);
	format.fmt.win.w.height=atoi(argv[5]);
	format.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;

	fd = open ((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDONLY) ;
	if (fd <= 0) {
		printf("Could not open %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
		return -1;
	}
	else
		printf("openned %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);

	ret = ioctl (fd, VIDIOC_S_FMT, &format);
	if (ret < 0) {
		perror ("VIDIOC_S_FMT");
		return 0;
	}

	show_info(fd);

	close(fd) ;
}
