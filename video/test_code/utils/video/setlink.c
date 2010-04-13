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
	printf("Usage: setlink <video_device> <0/1>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	int link ;

	if (argc < 3)
		return usage();

	video_device = atoi(argv[1]);
	link = atoi(argv[2]);

	if ((video_device != 1) && (video_device != 2)) {
		printf("video_device has to be 1 or 2!\n");
		return usage();
	}

	if (video_device == 0)
		return 0;

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

	result = ioctl(file_descriptor, VIDIOC_G_OMAP2_LINK);
	if (result < 0) {
		perror("VIDIOC_G_OMAP2_LINK");
		return 1;
	}
	printf("Current link status: V%d is %s linked to another layer\n",
		video_device, (result == 1) ? "" : "not");

	result = ioctl(file_descriptor, VIDIOC_S_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_S_OMAP2_LINK");
		return 1;
	}

	result = ioctl(file_descriptor, VIDIOC_G_OMAP2_LINK);
	if (result < 0) {
		perror("VIDIOC_G_OMAP2_LINK");
		return 1;
	}

	printf("Updated link status: V%d is %s linked to another layer\n",
		video_device, (result == 1) ? "" : "not");

	close(file_descriptor);
	return 0;
}
