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
	printf("Usage: setlink <video_device>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	int link;

	if (argc < 2)
		return usage();

	video_device = atoi(argv[1]);
	if ((video_device != 1) && (video_device != 2)) {
		printf("video_device has to be 1 or 2!\n");
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

	result = ioctl(file_descriptor, VIDIOC_G_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_G_OMAP2_LINK");
		return 1;
	}
	printf("link status: V%d is %s linked to another layer\n", video_device,
		(link == 1) ? "" : "not");

	link = 1;
	result = ioctl(file_descriptor, VIDIOC_S_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_S_OMAP2_LINK");
		return 1;
	}
	printf("linked!\n");

	result = ioctl(file_descriptor, VIDIOC_STREAMON, &link);
	if (result != 0) {
		perror("VIDIOC_STREAMON");
		return 1;
	}

	sleep(30);

	result = ioctl(file_descriptor, VIDIOC_STREAMOFF, &link);
	if (result != 0) {
		perror("VIDIOC_STREAMOFF");
		return 1;
	}
	
	link = 0;
	result = ioctl(file_descriptor, VIDIOC_S_OMAP2_LINK, &link);
	if (result != 0) {
		perror("VIDIOC_S_OMAP2_LINK");
		return 1;
	}
	printf("unlinked!\n");

	close(file_descriptor);
	return 0;
}
