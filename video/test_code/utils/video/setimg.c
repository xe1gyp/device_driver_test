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
	printf("Usage: setimg <video_device> <fmt> <width> <height>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	struct v4l2_format format;
	
	if (argc < 5)
		return usage();

	video_device = atoi(argv[1]);
	if ((video_device != 1) && (video_device != 2)) {
		printf("video_device has to be 1 or 2! "
			"video_device=%d, argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}

	file_descriptor =
		open((video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2,
		O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
		return 1;
	} else {
		printf("openned %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 : VIDEO_DEVICE2);
	}
	format.type           = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	result = ioctl(file_descriptor, VIDIOC_G_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_G_FMT");
		return 1;
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
		printf("fmt has to be YUYV, RGB565, RGB32, "
			"RGB24, UYVY or RGB565X!\n");
		return usage();
	}

	format.fmt.pix.width  = atoi(argv[3]);
	format.fmt.pix.height = atoi(argv[4]);
	

	/* set format of the picture captured */
	result = ioctl(file_descriptor, VIDIOC_S_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_S_FMT");
		return 1;
	}

	result = show_info(file_descriptor);

	close(file_descriptor);
	return result;
}
