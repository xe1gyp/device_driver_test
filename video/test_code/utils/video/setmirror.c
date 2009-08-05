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

#define  ENABLE  1
#define  DISABLE 0


static int usage(void)
{
	printf("Usage: mirroring <video_device> <enable[1]/disable[0]>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int video_device, file_descriptor, result;
	int state;
	struct v4l2_control control;

	memset(&control, 0, sizeof(control));
	control.id = V4L2_CID_VFLIP;
	control.value = DISABLE;

	if (argc < 3)
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

	state = atoi(argv[2]);

	if (state != 0)
		control.value = ENABLE;
	else
		control.value = DISABLE;

	result = ioctl(file_descriptor, VIDIOC_S_CTRL, &control);

	 if (result != 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}

	printf("Mirroring set to %d \n", state);
	close(file_descriptor);
	return 0;
}
