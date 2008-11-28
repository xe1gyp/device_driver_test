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
	printf("Usage: setimg <vid> <fmt> <width> <height>\n");
	return 0;
}

int main (int argc, char *argv[])
{
	int vid, fd, ret;	
	struct v4l2_format format;
	
	if (argc < 5)
		return usage();

	vid = atoi(argv[1]);
	if ((vid != 1) && (vid != 2)){
		printf("vid has to be 1 or 2! vid=%d, argv[1]=%s\n", vid, argv[1]);
		return usage();
	}

	if (!strcmp (argv[2], "YUYV"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	else if (!strcmp (argv[2], "UYVY"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
	else if (!strcmp (argv[2], "RGB565"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	else if (!strcmp (argv[2], "RGB565X"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565X;
	else if (!strcmp (argv[2], "RGB24"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	else if (!strcmp (argv[2], "RGB32"))
	    format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
	else {
		printf("fmt has to be YUYV, RGB565, RGB32, RGB24, UYVY or RGB565X!\n");
		return usage();
	}

	format.fmt.pix.width = atoi(argv[3]);
	format.fmt.pix.height = atoi(argv[4]);
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	
	fd = open ((vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2, O_RDONLY) ;
	if (fd <= 0) {
		printf("Could not open %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
		return -1;
	}
	else
		printf("openned %s\n", (vid == 1)?VIDEO_DEVICE1:VIDEO_DEVICE2);
	 
	/* set format of the picture captured */
	ret = ioctl (fd, VIDIOC_S_FMT, &format);
	if (ret < 0) {
		perror ("VIDIOC_S_FMT");
		return 0;
	}

	show_info(fd);

	close(fd) ;
}
