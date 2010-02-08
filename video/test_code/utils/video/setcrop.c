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
	printf("Usage: setcrop <video_device> <left> <top> <width> <height>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	struct v4l2_crop crop;

	if (argc < 6)
		return usage();

	video_device = atoi(argv[1]);
	if ((video_device != 1) && (video_device != 2) && (video_device != 3)) {
		printf("video_device has to be 1 or 2 or 3! video_device=%d, "
			"argv[1]=%s\n", video_device, argv[1]);
		return usage();
	}

	crop.c.left   = atoi(argv[2]);
	crop.c.top    = atoi(argv[3]);
	crop.c.width  = atoi(argv[4]);
	crop.c.height = atoi(argv[5]);
	crop.type     = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	file_descriptor =
		open((video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3),
		O_RDWR);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));
		return 1;
	} else
		printf("openned %s\n",
			(video_device == 1) ? VIDEO_DEVICE1 :
			((video_device == 2) ? VIDEO_DEVICE2 : VIDEO_DEVICE3));

	result = ioctl(file_descriptor, VIDIOC_S_CROP, &crop);
	if (result != 0) {
		perror("VIDIOC_S_CROP");
		return 1;
	}

	result = show_info(file_descriptor);
	close(file_descriptor);

	return result;
}
