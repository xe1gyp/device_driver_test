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
	printf("Usage: setbgcolor  <24 bit RGB value>\n");
	return 1;
}

int main(int argc, char *argv[])
{
	int file_descriptor, result;
	struct v4l2_control control;
	memset(&control, 0 , sizeof(control));
	control.id = V4L2_CID_BG_COLOR;

	if (argc < 2)
		return usage();

	file_descriptor = open(VIDEO_DEVICE1, O_RDONLY);
	if (file_descriptor <= 0) {
		printf("Could not open %s\n",VIDEO_DEVICE1);
		return 1;
	} else {
		printf("openned %s\n", VIDEO_DEVICE1);
	}

	control.value = atoi(argv[1]);

	result = ioctl(file_descriptor, VIDIOC_S_CTRL, &control);
	if (result != 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}

	printf("Back Ground color is set to %x \n", atoi(argv[1]));
	close(file_descriptor);

	return 0;
}
